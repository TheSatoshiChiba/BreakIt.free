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
#include "LoadingState.h"
#include "MenuState.h"
#include "Globals.h"

//Engine Includes
#include "GraphicsManager.h"

//Game Includes
#include "Logo.h"

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
using namespace BreakIt::GUI;

class LoadingState::Impl {
public:
	Impl();

	bool loadingStarted;
	bool loadingFinished;

	XMFLOAT2 texPos;
	std::unique_ptr<Logo> objLogo;

	//Music
	std::shared_ptr<Music> titleMusic;

	//Fonts
	std::shared_ptr<SpriteFont> segoeUIsemi20;
	std::shared_ptr<SpriteFont> tinyBoxBlackBitA8;

	//Methods
	void Initialize( GameManager *manager );

	void LoadData( GameManager *manager );
	bool IsLoadingFinished();
	bool MusicLoaded();
};//Impl class

LoadingState::Impl::Impl() {
}//Ctor()

void LoadingState::Impl::LoadData( GameManager *manager ) {
	if ( !loadingStarted ) {
		auto content = manager->GetMenuContent();
		//auto graphics = manager->GetGraphicsManager();
		auto audio = manager->GetAudioManager();

		//Preload Music
		titleMusic = content->LoadMusic( audio, MusicFilename.c_str() );

		loadingStarted = true;
	}
}//LoadData()

bool LoadingState::Impl::IsLoadingFinished() {
	if ( loadingStarted ) {
		if ( MusicLoaded() ) {
			return true;
		}
	}

	return false;
}//IsLoadingFinished()

bool LoadingState::Impl::MusicLoaded() {
	return titleMusic->IsInitialized();
}//MusicLoaded()

void LoadingState::Impl::Initialize( GameManager *manager ) {
	auto graphics		= manager->GetGraphicsManager();
	auto screenWidth	= graphics->GetViewportWidth();
	auto screenHeight	= graphics->GetViewportHeight();

	objLogo->Position = XMFLOAT2(
		screenWidth * 0.5f - 256.0f,
		screenHeight * 0.5f - 128.0f
		);

	XMFLOAT2 fsize;
	XMStoreFloat2( &fsize, segoeUIsemi20->MeasureString( L"Loading..." ) );
	texPos = XMFLOAT2( screenWidth * 0.5f - fsize.x * 0.5f, screenHeight * 0.5f + 260.0f );

	graphics->SetClearColor( XMVectorSet( 0.1f, 0.1f, 0.1f, 1.0f ) );
}//Initialize()

LoadingState::LoadingState() :
	pImpl( new Impl() ) {
}//Ctor()

LoadingState::~LoadingState() {
	pImpl = nullptr;
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( LoadingState );

void LoadingState::Load( GameManager *manager )
{
	GameState::Load( manager );

	pImpl->loadingStarted = false;
	pImpl->loadingFinished = false;

	auto graphics = manager->GetGraphicsManager();
	auto content = manager->GetMenuContent();

	//Create Logo
	pImpl->objLogo = std::make_unique<Logo>(
		0.0f,
		0.0f,
		512.0f,
		256.0f,
		content->LoadTexture2D( graphics, TextureFilename.c_str() )
		);

	//Load Loading Font
	pImpl->segoeUIsemi20 = content->LoadFont( graphics, FontFilename.c_str() );
	pImpl->tinyBoxBlackBitA8 = content->LoadFont( graphics, FontFilename2.c_str() );

	//Initialize Common Data
	pImpl->Initialize( manager );
}//Load()

void LoadingState::Unload() {
}//Unload()

void LoadingState::Pause() {
}//Pause()

void LoadingState::Resume() {
	pImpl->Initialize( gameManager );
}//Resume()

void LoadingState::Update( float elapsedTime, float totalTime ) {
	pImpl->LoadData( gameManager );
	if ( !pImpl->loadingFinished && pImpl->IsLoadingFinished() ) {
		//Loading finished so change the gamestate
		pImpl->loadingFinished = true;
		gameManager->ChangeGameState( GameState::Create<BreakIt::GameStates::MenuState>() );
		return;
	} else if ( pImpl->loadingFinished ) {
		//We are back from the menu state so delete this state
		gameManager->PopGameState();
		return;
	}
}//Update()

void LoadingState::Draw( float elapsedTime, float totalTime ) {
	auto sprites = gameManager->GetSpriteBatch();

	sprites->Begin();
	pImpl->objLogo->Draw( sprites );
	pImpl->segoeUIsemi20->DrawString( sprites, L"Loading...", XMLoadFloat2( &pImpl->texPos ), Colors::White, 0.0f, g_XMZero, 1.5f );
	sprites->End();
}//Draw()
