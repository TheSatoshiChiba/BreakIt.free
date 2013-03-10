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
#include "MenuState.h"
#include "MapLoadingState.h"
#include "BasicStyle.h"
#include "Globals.h"
#include "InfoState.h"
#include "HighscoreState.h"
#include "GameplayManager.h"

//Engine Includes
#include "GraphicsManager.h"

//Game Includes
#include "Logo.h"
#include "GUIManager.h"

//Using directives
using namespace DirectX;
using namespace WinGame;
using namespace WinGame::Audio;
using namespace WinGame::Content;
using namespace WinGame::Game;
using namespace WinGame::Graphics;
using namespace WinGame::Input;
using namespace BreakIt;
using namespace BreakIt::GameStates;
using namespace BreakIt::Objects;
using namespace BreakIt::Styles;
using namespace BreakIt::GUI;

class MenuState::Impl
{
public:
	Impl();

	//GameObjects
	std::unique_ptr<Logo>		objLogo;
	std::unique_ptr<GUIManager> guiManager;

	//Resources
	std::shared_ptr<Music>		titleMusic;
	std::shared_ptr<SpriteFont> font;

	//WinRT Stuff
	Platform::Agile<Windows::UI::Popups::MessageDialog> dlgResume;

	//Methods
	void LoadData( GameManager *manager );
	void InitializeData( GameManager *manager );

	float trialPosX[5];
};//Impl class

MenuState::Impl::Impl() {
}//Ctor()

void MenuState::Impl::LoadData( GameManager *manager )
{
	auto content	= manager->GetMenuContent();
	auto audio		= manager->GetAudioManager();
	auto graphics	= manager->GetGraphicsManager();

	//Create Logo
	objLogo = std::make_unique<Logo>(
		0.0f,
		0.0f,
		512.0f,
		256.0f,
		content->LoadTexture2D( graphics, TextureFilename.c_str() )
		);
	objLogo->Origin = XMFLOAT2( 256.0f, 128.0f );

	//Load TitleMusic
	titleMusic = content->LoadMusic( audio, MusicFilename.c_str() );

	//Create Buttons
	font = content->LoadFont( graphics, FontFilename2.c_str() );
	guiManager = std::make_unique<GUIManager>();
	guiManager->Initialize(
		content->LoadTexture2D( graphics, TextureFilename.c_str() ), 
		font,
		content->LoadFont( graphics, FontFilename.c_str() ), 
		graphics->IsWidescreen()
		);

	//Init WinRt Stuff
	dlgResume = ref new Windows::UI::Popups::MessageDialog( "Do you want to resume your last game?", "Resume last game?" );
	
	auto commYes = ref new Windows::UI::Popups::UICommand(
		"Yes",
		ref new Windows::UI::Popups::UICommandInvokedHandler( [=]( Windows::UI::Popups::IUICommand^ command ) {
			//Go to Map Loading state
			manager->SetStyleManager( std::make_unique<BasicStyle>() );
			auto mapLoading = GameState::Create<MapLoadingState, int, std::uint8_t, std::uint32_t>( 
				std::move( manager->levelIndex ), 
				std::move( manager->tempHealth ), 
				std::move( manager->tempPoints )
				);
			manager->PushGameState( std::move( mapLoading ) );
		} )
		);

	auto commNo = ref new Windows::UI::Popups::UICommand(
		"No",
		ref new Windows::UI::Popups::UICommandInvokedHandler( [=]( Windows::UI::Popups::IUICommand^ command ) {
			//TODO: Clicked no :)
		} )
		);

	dlgResume->Commands->Append( commYes );
	dlgResume->Commands->Append( commNo );
	dlgResume->DefaultCommandIndex = 0;
	dlgResume->CancelCommandIndex = 1;
}//LoadData()

void MenuState::Impl::InitializeData( GameManager *manager ) {
	auto vr			= manager->GetVirtualResolution();
	auto graphics	= manager->GetGraphicsManager();
	//auto content = manager->GetMenuContent();

	//Reset Level Index
	manager->GetGameplayManager()->SetLevelIndex( -1 );

	if( graphics->GetAspectRatio() <= 1.34f ) {
		//4:3 like screen
		vr->Width	= VirtualScreenWidth;
		vr->Height	= VirtualScreenHeight;
	} else {
		//16:9 like screen
		vr->Width	= VirtualWideScreenWidth;
		vr->Height	= VirtualWideScreenHeight;
	}

	//Set Logo Position
	objLogo->Position = XMFLOAT2(
		graphics->IsWidescreen() ? vr->Width * 0.5f : ( vr->Width - SidebarWidth ) * 0.5f,
		objLogo->Size.y * 0.5f + 25.0f
		);

	objLogo->AngleDifference = 15.0f;
	objLogo->ScaleDifference = 0.1f;
	
	//Set GUI
	guiManager->ClearButtons();

	XMFLOAT2 sizef;
	XMStoreFloat2( &sizef, font->MeasureString( L"START GAME" ) );
	sizef.x = ( vr->Width - sizef.x ) * 0.5f;
	sizef.y = ( vr->Height - sizef.y ) * 0.5f + 50.0f;

	if ( !graphics->IsWidescreen() ) {
		sizef.x -= SidebarWidth * 0.5f;
	}

	guiManager->AddButton(
		sizef.x,
		sizef.y,
		L"START GAME",
		[=]() {
			//Go to Map Loading state
			manager->SetStyleManager( std::make_unique<BasicStyle>() );
			auto mapLoading = GameState::Create<MapLoadingState, std::uint8_t>( 0U );
			manager->PushGameState( std::move( mapLoading ) );
		} );

	XMStoreFloat2( &sizef, font->MeasureString( L"HIGHSCORE" ) );

	sizef.x = ( vr->Width - sizef.x ) * 0.5f;
	sizef.y = ( vr->Height - sizef.y ) * 0.5f + 150.0f;

	if ( !graphics->IsWidescreen() ) {
		sizef.x -= SidebarWidth * 0.5f;
	}

	guiManager->AddButton(
		sizef.x,
		sizef.y,
		L"HIGHSCORE",
		[=]()
		{
			//Go to Highscore state
			auto high = GameState::Create<HighscoreState>();
			manager->PushGameState( std::move( high ) );
		} );

	XMStoreFloat2( &sizef, font->MeasureString( L"INFORMATION" ) );
	sizef.x = ( vr->Width - sizef.x ) * 0.5f;
	sizef.y = ( vr->Height - sizef.y ) * 0.5f + 250.0f;

	if ( !graphics->IsWidescreen() ) {
		sizef.x -= SidebarWidth * 0.5f;
	}

	guiManager->AddButton(
		sizef.x,
		sizef.y,
		L"INFORMATION",
		[=]()
		{
			//Go to Info state
			auto info = GameState::Create<InfoState>();
			manager->PushGameState( std::move( info ) );
		} );

	guiManager->AddButton(
		vr->Width - SidebarWidth + 20.0f,
		vr->Height - 42.0f,
		L".dandry",
		[=]()
		{
			auto uri = ref new Windows::Foundation::Uri( "http://www.drywa.me" );
			Windows::System::Launcher::LaunchUriAsync( uri );
		} );

	trialPosX[0] = -200.0f;
	trialPosX[1] = 100.0f;
	trialPosX[2] = 400.0f;
	trialPosX[3] = 700.0f;
	trialPosX[4] = 1000.0f;

	guiManager->Resize( graphics->IsWidescreen() );
	titleMusic->Play( 0.5f );
	graphics->SetClearColor( XMVectorSet( 0.1f, 0.1f, 0.1f, 1.0f ) );
}//InitializeData()

MenuState::MenuState() :
	pImpl( new Impl() ) {
}//Ctor()

MenuState::~MenuState() {
	pImpl = nullptr;
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( MenuState );

void MenuState::Load( GameManager *manager ) {
	GameState::Load( manager );
	
	pImpl->LoadData( manager );
	pImpl->InitializeData( manager );
}//Load()

void MenuState::Unload() {
	pImpl->titleMusic->Stop();
}//Unload()

void MenuState::Pause() {
}//Pause()

void MenuState::Resume() {
	pImpl->InitializeData( gameManager );
}//Resume()

void MenuState::Update( float elapsedTime, float totalTime )
{
	static bool switchedMode	= false;
	static bool isShown			= false;
	if( gameManager->isResumed && !isShown )
	{
		std::wstringstream str;
		str << L"Do you wan't to resume your last played game? (Level: "
			<< gameManager->levelIndex + 1
			<< ", Lifes: " << gameManager->tempHealth
			<< ", Points: " << gameManager->tempPoints
			<< ")";
		std::wstring cstr( str.str() );
		pImpl->dlgResume->Content = ref new Platform::String( cstr.c_str() );
		pImpl->dlgResume->ShowAsync();
		isShown = true;
	}

	if ( !switchedMode ) {
		Pause();
		Resume();
		switchedMode = true;
	}

	pImpl->titleMusic->Update();
	pImpl->objLogo->Update( elapsedTime, totalTime );
	pImpl->guiManager->Update( gameManager );
}//Update()

void MenuState::Draw( float elapsedTime, float totalTime ) {
	auto sprites	= gameManager->GetSpriteBatch();
	auto graphics	= gameManager->GetGraphicsManager();
	auto vr			= gameManager->GetVirtualResolution();

	vr->SetVirtualViewport( graphics );

	sprites->Begin(
		SpriteSortMode::SpriteSortMode_Deferred,
		nullptr, 
		nullptr, //graphics->GetCommonStates()->LinearClamp(), 
		nullptr, 
		nullptr, //graphics->GetCommonStates()->Wireframe(),
		nullptr, 
		vr->GetScaleMatrix( graphics )
		);

	//Draw Background
	pImpl->guiManager->DrawRectangle(
		sprites, 
		graphics->IsWidescreen() ? SplitterWidth + SidebarWidth : SplitterWidth,
		0.0f,
		GameFieldWidth,
		vr->Height,
		XMVectorSet( 0.1f, 0.1f, 0.1f, 1.0f )
		);

	//Draw Game Objects + GUI
	pImpl->objLogo->Draw( sprites );
	pImpl->guiManager->Draw( sprites, true );

	auto name = gameManager->GetHighscore()->GetUserName()->Data();
	pImpl->guiManager->DrawGameText( sprites, 0.0f, 0.0f, 0.0f, vr->Width - SidebarWidth + 15.0f, 15.0f, 0.5f, L"User: " );
	pImpl->guiManager->DrawGameText( sprites, 0.392156899f, 0.584313750f, 0.929411829f, vr->Width - SidebarWidth + 20.0f, 10.0f, 0.5f, L"User: " );
	pImpl->guiManager->DrawGameText( sprites, 0.0f, 0.0f, 0.0f, vr->Width - SidebarWidth + 15.0f, 35.0f, 0.5f, name );
	pImpl->guiManager->DrawGameText( sprites, 1.000000000f, 0.270588249f, 0.000000000f, vr->Width - SidebarWidth + 20.0f, 30.0f, 0.5f, name );

	sprites->End();
}//Draw()
