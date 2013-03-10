/*
====================================================================
The MIT License

Break It - Copyright (C) 2013 by Daniel Drywa (daniel@drywa.me)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies 
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED 
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
===================================================================
*/

//Essentials
#include "pch.h"
#include "GameManager.h"
#include "GameplayState.h"
#include "InputManager.h"
#include "BasicStyle.h"
#include "Globals.h"

//Gameplay specific
#include "GUIManager.h"
#include "GameplayManager.h"
#include "Logo.h"

using namespace BreakIt;
using namespace BreakIt::Objects;
using namespace BreakIt::GameStates;
using namespace BreakIt::GUI;
using namespace BreakIt::Styles;
using namespace WinGame::Game;
using namespace WinGame::Graphics;
using namespace WinGame::Input;
using namespace WinGame::Audio;
using namespace DirectX;

struct ItemObject {
	int count;
	ITEM_TYPES type;

	ItemObject( int c, ITEM_TYPES t ) {
		count = c;
		type = t;
	}

	bool operator< ( const ItemObject& rh ) {
		return count < rh.count;
	}
};

ref class EdgeEvent sealed {
public:
	EdgeEvent() :
		isActive( false ),
		isListening( true ),
		oneTimeActive( false ) {

		Windows::UI::Input::EdgeGesture^ edgeGesture = Windows::UI::Input::EdgeGesture::GetForCurrentView();

		startingToken = edgeGesture->Starting += ref new 
			Windows::Foundation::TypedEventHandler<Windows::UI::Input::EdgeGesture^, Windows::UI::Input::EdgeGestureEventArgs^>(
			this, &EdgeEvent::OnEdgeGestureStarting
			);
    
		completedToken = edgeGesture->Completed += ref new 
			Windows::Foundation::TypedEventHandler<Windows::UI::Input::EdgeGesture^, Windows::UI::Input::EdgeGestureEventArgs^>(
			this, &EdgeEvent::OnEdgeGestureCompleted);
    
		canceledToken = edgeGesture->Canceled += ref new 
			Windows::Foundation::TypedEventHandler<Windows::UI::Input::EdgeGesture^, Windows::UI::Input::EdgeGestureEventArgs^>(
			this, &EdgeEvent::OnEdgeGestureCanceled);
	}//Ctor()

	virtual ~EdgeEvent() {
		Windows::UI::Input::EdgeGesture^ edgeGesture = Windows::UI::Input::EdgeGesture::GetForCurrentView();

		edgeGesture->Starting -= startingToken;
		edgeGesture->Completed -= completedToken;
		edgeGesture->Canceled -= canceledToken;
	}//Dtor()

	bool IsActive() {
		return isActive;
	}//IsActive()

	void SetActive( bool active ) {
		isActive = active;
	}//SetActive()

	void ActivateOnce() {
		if ( !oneTimeActive ) {
			isActive = true;
			oneTimeActive = true;
		}
	}

	void Flush() {
		oneTimeActive = false;
	}

	void SetListening( bool listen ) {
		isListening = listen;
	}//SetListening()

private:
	bool oneTimeActive;
	bool isActive;
	bool isListening;
	Windows::Foundation::EventRegistrationToken startingToken;
	Windows::Foundation::EventRegistrationToken completedToken;
	Windows::Foundation::EventRegistrationToken canceledToken;

	void OnEdgeGestureStarting(
		Windows::UI::Input::EdgeGesture^ sender, 
		Windows::UI::Input::EdgeGestureEventArgs^ args
		)
	{
	}//OnEdgeGestureStarting()

	void OnEdgeGestureCompleted(
		Windows::UI::Input::EdgeGesture^ sender, 
		Windows::UI::Input::EdgeGestureEventArgs^ args
		)
	{
		if ( isListening ) {
			isActive = !isActive;
		}
	}//OnEdgeGestureCompleted()

	void OnEdgeGestureCanceled(
		Windows::UI::Input::EdgeGesture^ sender, 
		Windows::UI::Input::EdgeGestureEventArgs^ args
		)
	{
	}//OnEdgeGestureCanceled()

};//EdgeEvent class

class GameplayState::Impl
{
public:
	Impl();

	//GUI
	std::unique_ptr<GUIManager> guiManager;
	std::unique_ptr<Logo> objLogo;
	std::shared_ptr<SpriteFont> font;

	//Music
	std::shared_ptr<Music> titleMusic;

	//Methods
	XMVECTOR GetPlayerDelta( InputManager *input );

	//WinRT Stuff
	Platform::Agile<Windows::UI::Popups::MessageDialog> dlgQuit;

	std::vector<ItemObject> items;
	void LoadData( GameManager *manager );
	void InitializeData( GameManager *manager );
	EdgeEvent^ edgeEvent;

	bool isAdded;
	bool isButtonHit;
	void AddButton( float width );
};//GameplayState::Impl class

GameplayState::Impl::Impl() :
	edgeEvent( ref new EdgeEvent() ),
	isAdded( false ),
	isButtonHit( false ) {
}//Ctor()

void GameplayState::Impl::LoadData( GameManager *manager ) {
	auto audio = manager->GetAudioManager();
	auto graphics = manager->GetGraphicsManager();
	auto content = manager->GetMenuContent();
	//auto vr = manager->GetVirtualResolution();
	//auto style = manager->GetStyleManager();

	//Load TitleMusic
	titleMusic = content->LoadMusic( audio, MusicFilename.c_str() );

	//Init GUIManager
	font = content->LoadFont(
			graphics, 
			FontFilename2.c_str()
			);

	guiManager = std::make_unique<GUIManager>();
	guiManager->Initialize(
		content->LoadTexture2D(
			graphics, 
			TextureFilename.c_str()
			),
		font,
		content->LoadFont(
			graphics, 
			FontFilename.c_str()
			),
		graphics->IsWidescreen()
		);

	objLogo = std::make_unique<Logo>(
		0.0f,
		0.0f,
		512.0f,
		256.0f,
		content->LoadTexture2D( graphics, TextureFilename.c_str() )
		);
	objLogo->Origin = XMFLOAT2( 256.0f, 128.0f );
	objLogo->Size = XMFLOAT2( 200.0f, 100.0f );

	//Init WinRt Stuff
	dlgQuit = ref new Windows::UI::Popups::MessageDialog( "Do you really want to quit the game and go back to the main menu?", "Back to main menu?" );

	auto commYes = ref new Windows::UI::Popups::UICommand(
		"Yes",
		ref new Windows::UI::Popups::UICommandInvokedHandler( [=]( Windows::UI::Popups::IUICommand^ command ) {
			//Go to Main Menu
			auto level = manager->GetGameplayManager();
			auto p = level->GetPlayer();
			if ( level->IsGameWon() ) {
				p->TempPoints = p->Points;
				p->TempHealth = p->Health;
			} else if ( level->IsGameLost() ) {
			} else {
				p->Points = p->TempPoints;
				p->Health = p->TempHealth;
			}

			manager->GetGameplayManager()->QuitGame();
			manager->PopGameState(); 
		} )
		);

	auto commNo = ref new Windows::UI::Popups::UICommand(
		"No",
		ref new Windows::UI::Popups::UICommandInvokedHandler( [=]( Windows::UI::Popups::IUICommand^ command ) {
			isButtonHit = false;
		} )
		);

	dlgQuit->Commands->Append( commYes );
	dlgQuit->Commands->Append( commNo );
	dlgQuit->DefaultCommandIndex = 0;
	dlgQuit->CancelCommandIndex = 1;
	isAdded = false;
	isButtonHit = false;
}//LoadData()

void GameplayState::Impl::InitializeData( GameManager *manager )
{
	auto vr = manager->GetVirtualResolution();
	auto graphics = manager->GetGraphicsManager();
	
	if ( !graphics->IsWidescreen() ) {
		//4:3 like screen
		vr->Width = VirtualScreenWidth;
		vr->Height = VirtualScreenHeight;
	} else {
		//16:9 like screen
		vr->Width = VirtualWideScreenWidth;
		vr->Height = VirtualWideScreenHeight;
	}

	//Re-Set Static Text
	guiManager->ClearStaticText();
	guiManager->AddStaticText( vr->Width - SidebarWidth + 38.0f, 22.0f, Colors::Black, L" x " );
	guiManager->AddStaticText( vr->Width - SidebarWidth + 40.0f, 20.0f, Colors::CornflowerBlue, L" x " );

	guiManager->AddStaticText( vr->Width - SidebarWidth + 38.0f, 52.0f, Colors::Black, L" x " );
	guiManager->AddStaticText( vr->Width - SidebarWidth + 40.0f, 50.0f, Colors::CornflowerBlue, L" x " );
	
	//Buttons
	//guiManager->ClearButtons();
	//auto gameplay = manager->GetGameplayManager();
	//if(gameplay->IsGameLost() || gameplay->IsGamePaused() || gameplay->IsGameWon())
	//{
	//	//TODO: Add Back Button
	//}

	//Logo
	objLogo->Position = XMFLOAT2(
		SidebarWidth * 0.5f,
		80.0f
		);

	objLogo->AngleDifference = 15.0f;
	objLogo->ScaleDifference = 0.1f;

	//Resize
	guiManager->Resize( graphics->IsWidescreen() );
	titleMusic->Play( 0.5f );
	graphics->SetClearColor( XMVectorSet( 0.1f, 0.1f, 0.1f, 1.0f ) );
}//Initialize()

XMVECTOR GameplayState::Impl::GetPlayerDelta( InputManager *input ) {
	if ( input->IsFingerInContact() ) {
		return XMVectorSwizzle<0, 2, 2, 3>(
			input->GetFingerPositionDelta()
			);
	}

	return XMVectorSwizzle<0, 2, 2, 3>(
		input->GetMousePositionDelta()
		);

}//GetPlayerDelta()

void GameplayState::Impl::AddButton( float width ) {
	if ( !isAdded ) {
		isAdded = true;

		XMFLOAT2 fontSize;
		XMStoreFloat2(&fontSize, font->MeasureString(L"BACK TO MENU"));
		float fwidth = fontSize.x;

		guiManager->AddButton(
			( width - fwidth ) * 0.5f, 
			static_cast<float>( GameFieldHeight ) - 150.0f,
			L"BACK TO MENU",
			[=] () {
				isButtonHit = true;
				dlgQuit->ShowAsync();
			} 
			);
	}
}

GameplayState::GameplayState() :
	pImpl( new Impl() ) {
}//Ctor()

GameplayState::~GameplayState() {
	pImpl = nullptr;
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( GameplayState );

void GameplayState::Load( GameManager *manager ) {
	GameState::Load( manager );
	gameManager->GetInputManager()->HideMousePointer();

	pImpl->LoadData( manager );
	pImpl->InitializeData( manager );
}//Load()

void GameplayState::Unload() {
	gameManager->GetInputManager()->ShowMousePointer();
	pImpl->titleMusic->Stop();
}//Unload()

void GameplayState::Pause() {
	pImpl->edgeEvent->SetListening( false );
	gameManager->GetInputManager()->ShowMousePointer();
}//Pause()

void GameplayState::Resume() {
	auto graphics = gameManager->GetGraphicsManager();

	gameManager->GetGameplayManager()->Resize( graphics->IsWidescreen() );
	if ( gameManager->GetGameplayManager()->IsGamePaused() || gameManager->GetGameplayManager()->IsGameLost() || gameManager->GetGameplayManager()->IsGameWon() ) {
		if ( gameManager->GetGameplayManager()->IsGamePaused() ) {
			pImpl->edgeEvent->SetListening( true );
		}

		gameManager->GetInputManager()->ShowMousePointer();
	} else {
		gameManager->GetInputManager()->HideMousePointer();
	}

	pImpl->InitializeData( gameManager );
	
}//Resume()

void GameplayState::Update( float elapsedTime, float totalTime ) {
	auto input	= gameManager->GetInputManager();
	auto level	= gameManager->GetGameplayManager();
	auto vr		= gameManager->GetVirtualResolution();
	
	pImpl->titleMusic->Update();
	pImpl->objLogo->Update( elapsedTime, totalTime );
	pImpl->guiManager->Update( gameManager );

	if ( level->IsGamePaused() ) {
		input->ShowMousePointer();
		pImpl->edgeEvent->SetListening( true );
		pImpl->edgeEvent->ActivateOnce();
		pImpl->AddButton( vr->Width );

		if ( !pImpl->edgeEvent->IsActive() || 
			input->IsFingerNotInContactOnce() || 
			input->IsLeftMouseButtonReleasedOnce() || 
			input->IsRightMouseButtonReleasedOnce() ) {
			
			if ( !pImpl->isButtonHit ) {
				pImpl->guiManager->ClearButtons();
				pImpl->edgeEvent->SetActive( false );
				level->SetPause( false );
				return;
			}
		}

	} else if ( level->IsGameLost() || level->IsGameWon() ) {
		input->ShowMousePointer();
		pImpl->edgeEvent->SetListening( false );
		pImpl->AddButton( vr->Width );

		if ( level->IsGameWon() ) {
			auto p = level->GetPlayer();
			p->TempPoints = p->Points;
			p->TempHealth = p->Health;
		}

		if ( input->IsFingerNotInContactOnce() || 
			input->IsLeftMouseButtonReleasedOnce() || 
			input->IsRightMouseButtonReleasedOnce() ) {

			if ( !pImpl->isButtonHit ) {
				pImpl->guiManager->ClearButtons();
				gameManager->PopGameState();
				return;
			}
		}

	} else {
		pImpl->isAdded = false;
		input->HideMousePointer();
		pImpl->edgeEvent->SetListening( true );
		pImpl->edgeEvent->Flush();
		pImpl->isButtonHit = false;

		if ( pImpl->edgeEvent->IsActive() || input->IsRightMouseButtonReleasedOnce() ) {
			level->SetPause( true ); 
		}
	}

	level->Update( pImpl->GetPlayerDelta( input ), elapsedTime, totalTime );
}//Update()

void GameplayState::Draw( float elapsedTime, float totalTime ) {
	auto sprites = gameManager->GetSpriteBatch();
	auto graphics = gameManager->GetGraphicsManager();
	auto vr = gameManager->GetVirtualResolution();

	vr->SetVirtualViewport( graphics );

	sprites->Begin(
		SpriteSortMode::SpriteSortMode_Deferred,
		nullptr, //graphics->GetCommonStates()->AlphaBlend(),
		nullptr, //graphics->GetCommonStates()->LinearClamp(), 
		nullptr, 
		nullptr, //graphics->GetCommonStates()->Wireframe(),
		nullptr, 
		vr->GetScaleMatrix( graphics )
		);

	auto level = gameManager->GetGameplayManager();
	auto player = level->GetPlayer();
	auto balls = level->GetBalls();
	auto items = level->GetItems();
	//auto bricks = level->GetBricks();
	auto gui = pImpl->guiManager.get();
	auto style = gameManager->GetStyleManager();

	//Draw Background
	gui->DrawRectangle(
		sprites, 
		graphics->IsWidescreen() ? SplitterWidth + SidebarWidth : SplitterWidth,
		0.0f,
		GameFieldWidth,
		vr->Height,
		XMVectorSet( 0.1f, 0.1f, 0.1f, 1.0f )
		);

	//Draw Gameplay
	level->Draw( sprites );

	//Draw GUI
	gui->Draw( sprites, true );
	
	//Draw Logo
	if ( vr->IsWidescreen() )
		pImpl->objLogo->Draw( sprites );

	//Draw Items
	float posY = 10.0f;
	XMFLOAT3 cornflower;
	XMFLOAT3 black( 0.0f, 0.0f, 0.0f );
	XMStoreFloat3( &cornflower, Colors::CornflowerBlue );

	items->DrawStatic( sprites, ITEM_TYPES::COIN, vr->Width - SidebarWidth + 10.0f, posY );
	gui->DrawGameText( sprites, black.x, black.y, black.z, vr->Width - SidebarWidth + 68.0f, posY + 12.0f, 0.5f, L"%d", player->Points );
	gui->DrawGameText( sprites, cornflower.x, cornflower.y, cornflower.z, vr->Width - SidebarWidth + 70.0f, posY + 10.0f, 0.5f, L"%d", player->Points );

	posY += 30.0f;
	items->DrawStatic( sprites, ITEM_TYPES::HEART, vr->Width - SidebarWidth + 10.0f, posY );
	gui->DrawGameText( sprites, black.x, black.y, black.z, vr->Width - SidebarWidth + 68.0f, posY + 12.0f, 0.5f, L"%d", player->Health );
	gui->DrawGameText( sprites, cornflower.x, cornflower.y, cornflower.z, vr->Width - SidebarWidth + 70.0f, posY + 10.0f, 0.5f, L"%d", player->Health );

	posY += 30.0f;
	auto sc = level->GetBricks()->GetCount();
	gui->DrawGameText( sprites, black.x, black.y, black.z, vr->Width - SidebarWidth + 8.0f, posY + 12.0f, 0.5f, L"Bricks: %d", sc );
	gui->DrawGameText( sprites, cornflower.x, cornflower.y, cornflower.z, vr->Width - SidebarWidth + 10.0f, posY + 10.0f, 0.5f, L"Bricks: %d", sc );

	posY += 30.0f;
	gui->DrawGameText( sprites, black.x, black.y, black.z, vr->Width - SidebarWidth + 8.0f, posY + 12.0f, 0.5f, L"Level: %d / %d", level->GetLevelIndex() + 1, style->GetLevelCount() );
	gui->DrawGameText( sprites, cornflower.x, cornflower.y, cornflower.z, vr->Width - SidebarWidth + 10.0f, posY + 10.0f, 0.5f, L"Level: %d / %d", level->GetLevelIndex() + 1, style->GetLevelCount() );

	posY += 30.0f;
	pImpl->items.clear();

	if ( player->IsLaserActive() ) {
		pImpl->items.push_back( ItemObject( player->GetLaserCount(), ITEM_TYPES::LASER_GUN ) );
	}

	if ( balls->IsWallActive() ) {
		pImpl->items.push_back( ItemObject( balls->GetWallTime(), ITEM_TYPES::STEEL_WALL ) );
	}

	if ( balls->IsInvisible() ) {
		pImpl->items.push_back( ItemObject( balls->GetVisibleTime(), ITEM_TYPES::INVISIBLE_BALL ) );
	}

	if ( balls->IsBouncy() || balls->IsPowerful() ) {
		pImpl->items.push_back( ItemObject( balls->GetPowerTime(), balls->IsBouncy() ? ITEM_TYPES::SOFT_BALL : ITEM_TYPES::DEATH_BALL ) );
	}

	std::sort( std::begin( pImpl->items ), std::end( pImpl->items ) );
	for ( const auto &obj : pImpl->items ) {
		posY += 30.0f;
		items->DrawStatic( sprites, obj.type, vr->Width - SidebarWidth + 10.0f, posY );
		gui->DrawGameText( sprites, black.x, black.y, black.z, vr->Width - SidebarWidth + 38.0f, posY + 12.0f, 0.5f, L" x %d", obj.count );
		gui->DrawGameText( sprites, cornflower.x, cornflower.y, cornflower.z, vr->Width - SidebarWidth + 40.0f, posY + 10.0f, 0.5f, L" x %d", obj.count );
	}

	std::wstring text;
	XMVECTOR color = Colors::White;
	if ( level->IsGamePaused() ) {
		text = L"GAME PAUSED! CLICK/TAP TO PLAY!";
		color = Colors::CornflowerBlue;
	} else if ( level->IsGameLost() ) {
		text = L"GAME LOST! CLICK/TAP TO PLAY AGAIN!";
		color = Colors::Red;
	} else if ( level->IsGameWon() ) {
		text = L"GAME WON! CLICK/TAP TO PLAY NEXT!";
		color = Colors::Green;
	}
	XMStoreFloat3( &cornflower, color );

	if ( !text.empty() ) {
		auto width = gui->GetGameTextWidth( text.c_str() );
		gui->DrawGameText( sprites, black.x, black.y, black.z, (vr->Width - width) * 0.5f - 5.0f, (vr->Height - 24.0f) * 0.5f + 5.0f, 1.0f, text.c_str() );
		gui->DrawGameText( sprites, cornflower.x, cornflower.y, cornflower.z, (vr->Width - width) * 0.5f, (vr->Height - 24.0f) * 0.5f, 1.0f, text.c_str() );
	}

#if _DEBUG
	XMStoreFloat3( &cornflower, Colors::White );
	gui->DrawText( sprites, cornflower.x, cornflower.y, cornflower.z, 0.0f, 0.0f, L"FPS: %d", gameManager->GetFPS() );
	gui->DrawText( sprites, cornflower.x, cornflower.y, cornflower.z, 0.0f, 20.0f, L"Time: %fms", elapsedTime );
	//gui->DrawText(sprites, cornflower.x, cornflower.y, cornflower.z, 0.0f, 40.0f, L"PreHealth: %d", level->GetPlayer()->TempHealth);
	//gui->DrawText(sprites, cornflower.x, cornflower.y, cornflower.z, 0.0f, 60.0f, L"PrePoints: %d", level->GetPlayer()->TempPoints);
#endif

	sprites->End();

}//Draw()
