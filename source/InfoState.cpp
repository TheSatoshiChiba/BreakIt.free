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
#include "InfoState.h"
#include "InputManager.h"
#include "BasicStyle.h"
#include "Globals.h"

//Gameplay specific
#include "GUIManager.h"
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

class InfoState::Impl {
public:
	Impl();

	//GameObjects
	std::unique_ptr<GUIManager> guiManager;

	//Resources
	std::shared_ptr<Music> titleMusic;
	std::shared_ptr<SpriteFont> font;

	//Methods
	void LoadData( GameManager *manager );
	void InitializeData( GameManager *manager );

	void AddText( float y, bool widescreen, XMVECTOR color, const wchar_t *text );
};//InfoState::Impl class

InfoState::Impl::Impl() {
}//Ctor()

void InfoState::Impl::LoadData( GameManager *manager ) {
	auto content	= manager->GetMenuContent();
	auto audio		= manager->GetAudioManager();
	auto graphics	= manager->GetGraphicsManager();

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
}//LoadData()

void InfoState::Impl::AddText( float y, bool widescreen, XMVECTOR color, const wchar_t *text )
{
	float width = widescreen ? static_cast<float>( VirtualWideScreenWidth ) : static_cast<float>( VirtualScreenWidth );
	XMFLOAT2 sizef;
	XMStoreFloat2( &sizef, XMVectorScale( font->MeasureString( text ), 0.5f ) );
	sizef.x = ( width - sizef.x ) * 0.5f;
	sizef.y = y;
	if ( !widescreen ) {
		sizef.x -= SidebarWidth * 0.5f;
	}
	guiManager->AddStaticText( sizef.x - 5.0f, sizef.y + 5.0f, Colors::Black, text );
	guiManager->AddStaticText( sizef.x, sizef.y, color, text );
}//AddText()

void InfoState::Impl::InitializeData( GameManager *manager ) {
	auto vr			= manager->GetVirtualResolution();
	auto graphics	= manager->GetGraphicsManager();
	//auto content = manager->GetMenuContent();

	if ( graphics->GetAspectRatio() <= 1.34f ) {
		//4:3 like screen
		vr->Width = VirtualScreenWidth;
		vr->Height = VirtualScreenHeight;
	} else {
		//16:9 like screen
		vr->Width = VirtualWideScreenWidth;
		vr->Height = VirtualWideScreenHeight;
	}
	
	//Set GUI
	guiManager->ClearButtons();

	XMFLOAT2 sizef;
	XMStoreFloat2( &sizef, font->MeasureString( L"BACK" ) );
	sizef.x = ( vr->Width - sizef.x ) * 0.5f;
	sizef.y = ( vr->Height - sizef.y ) - 20.0f;

	if ( !graphics->IsWidescreen() ) {
		sizef.x -= SidebarWidth * 0.5f;
	}

	guiManager->AddButton(
		sizef.x,
		sizef.y,
		L"BACK",
		[=]() {
			manager->PopGameState();	
		} );

	AddText( 20.0f, graphics->IsWidescreen(), Colors::Red, L"Thank you for playing \"Break It\"! :)" );

	AddText( 60.0f, graphics->IsWidescreen(), Colors::CornflowerBlue, L"Code / SFX / GFX:" );
	AddText( 80.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"Daniel Drywa" );
	AddText( 100.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"(daniel@drywa.me)" );

	AddText( 140.0f, graphics->IsWidescreen(), Colors::CornflowerBlue, L"Music:" );
	AddText( 160.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"Psykick" );
	AddText( 180.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"(http://ccmixter.org/files/Psykick/37386)" );

	AddText( 220.0f, graphics->IsWidescreen(), Colors::CornflowerBlue, L"Winning/Losing SFX:" );
	AddText( 240.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"sauer2" );
	AddText( 260.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"(http://opengameart.org/content/" );
	AddText( 280.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"oldschool-win-and-die-jump-and-run-sounds)" );

	AddText( 320.0f, graphics->IsWidescreen(), Colors::CornflowerBlue, L"Special Thanks:" );
	AddText( 340.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"Kevin Zastrow - SFX for the Alpha Version" );
	AddText( 360.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"Omar Ajerray - Some Maps + Testing" );

	AddText( 400.0f, graphics->IsWidescreen(), Colors::CornflowerBlue, L"\"Break It\" Version 1.2" );
	AddText( 420.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"This is the final version of the Game and there" );
	AddText( 440.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"won't be any more updates since I am working on" );
	AddText( 460.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"a new Project. This Game is now free forever." );
	AddText( 480.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"To all the People who paid for the first Version" );
	AddText( 500.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"of the Game: THANK YOU! You are AWESOME!" );
	AddText( 520.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"Please visit my website for any news and updates" );
	AddText( 540.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"about my new Project. Check it out and I" );
	AddText( 560.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"hope to see you there." );

	AddText( 600.0f, graphics->IsWidescreen(), Colors::OrangeRed, L"Thanks and Have Fun! :)" );

	guiManager->Resize( graphics->IsWidescreen() );
	titleMusic->Play( 0.5f );
	graphics->SetClearColor( XMVectorSet( 0.1f, 0.1f, 0.1f, 1.0f ) );
}//Initialize()

InfoState::InfoState() :
	pImpl( new Impl() ) {
}//Ctor()

InfoState::~InfoState() {
	pImpl = nullptr;
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( InfoState );

void InfoState::Load( GameManager *manager ) {
	GameState::Load( manager );

	pImpl->LoadData( manager );
	pImpl->InitializeData( manager );
}//Load()

void InfoState::Unload() {
	pImpl->titleMusic->Stop();
}//Unload()

void InfoState::Pause() {
}//Pause()

void InfoState::Resume() {
	pImpl->InitializeData( gameManager );
}//Resume()

void InfoState::Update( float elapsedTime, float totalTime ) {
	pImpl->titleMusic->Update();
	pImpl->guiManager->Update( gameManager );
}//Update()

void InfoState::Draw( float elapsedTime, float totalTime ) {
	auto sprites	= gameManager->GetSpriteBatch();
	auto graphics	= gameManager->GetGraphicsManager();
	auto vr			= gameManager->GetVirtualResolution();

	vr->SetVirtualViewport(graphics);

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
	pImpl->guiManager->Draw( sprites, true );
	sprites->End();

}//Draw()
