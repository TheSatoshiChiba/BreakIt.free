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
#include "MapLoadingState.h"
#include "GameplayState.h"
#include "Globals.h"
#include "HighscoreState.h"

//Engine Includes
#include "GraphicsManager.h"

//Gameplay Includes
#include "Logo.h"
#include "GameplayManager.h"

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

class MapLoadingState::Impl {
public:
	Impl( std::uint8_t index );
	Impl( int levelIndex, std::uint8_t health, std::uint32_t points ); 

	std::uint8_t levelIndex;
	std::uint8_t health;
	std::uint32_t points;
	bool isResumed;

	bool contentLoadingStarted;
	bool mapLoadingStarted;

	XMFLOAT2 texPos;
	std::unique_ptr<Logo> objLogo;
	std::shared_ptr<SpriteFont> segoeUIsemi20;

	//Textures
	std::vector<std::shared_ptr<Texture2D>> Textures;

	//Sounds
	std::vector<std::shared_ptr<Sound>> Sounds;

	//Methods
	void Initialize( GameManager *manager );
	bool LoadContent( GameManager *manager );
	bool LoadMap( GameManager *manager );
};//Impl class

MapLoadingState::Impl::Impl( std::uint8_t index ) :
	levelIndex( index ),
	isResumed( false ) {
}//Ctor()

MapLoadingState::Impl::Impl( int levelIndex, std::uint8_t health, std::uint32_t points ) :
	levelIndex( static_cast<std::uint8_t>( levelIndex ) ),
	points( points ),
	health( health ),
	isResumed( true ) {
}//Ctor()

void MapLoadingState::Impl::Initialize( GameManager *manager ) {
	auto graphics		= manager->GetGraphicsManager();
	auto screenWidth	= graphics->GetViewportWidth();
	auto screenHeight	= graphics->GetViewportHeight();

	objLogo->Position = XMFLOAT2(
		screenWidth * 0.5f - 256.0f,
		screenHeight * 0.5f - 128.0f
		);

	XMFLOAT2 fsize;
	XMStoreFloat2( &fsize, segoeUIsemi20->MeasureString( L"LOADING DATA" ) );
	texPos = XMFLOAT2( screenWidth * 0.5f - fsize.x, screenHeight * 0.5f + 260.0f );

	graphics->SetClearColor( XMVectorSet( 0.1f, 0.1f, 0.1f, 1.0f ) );
}//Initialize()

bool MapLoadingState::Impl::LoadContent( GameManager *manager ) {
	auto content	= manager->GetGameplayContent();
	auto graphics	= manager->GetGraphicsManager();
	auto audio		= manager->GetAudioManager();
	auto style		= manager->GetStyleManager();

	if ( !contentLoadingStarted ) {
		//Load Content
		Textures.clear();
		Textures.push_back( content->LoadTexture2D( graphics, style->GetStyleTexture() ) );
		
		Sounds.clear();
		Sounds.push_back( content->LoadSound( audio, style->GetBallLostSound() ) );
		Sounds.push_back( content->LoadSound( audio, style->GetBrickBreakSound() ) );
		Sounds.push_back( content->LoadSound( audio, style->GetBrickHitSound() ) );
		Sounds.push_back( content->LoadSound( audio, style->GetPointsAddSound() ) );
		Sounds.push_back( content->LoadSound( audio, style->GetGameLostSound() ) );
		Sounds.push_back( content->LoadSound( audio, style->GetGameWonSound() ) );
		Sounds.push_back( content->LoadSound( audio, style->GetHealthAddSound() ) );
		Sounds.push_back( content->LoadSound( audio, style->GetPlayerHitSound() ) );
		Sounds.push_back( content->LoadSound( audio, style->GetSideHitSound() ) );
		Sounds.push_back( content->LoadSound( audio, style->GetNegativeItemSound() ) );
		Sounds.push_back( content->LoadSound( audio, style->GetPositiveItemSound() ) );

		contentLoadingStarted = true;
	} else {
		//Return true if everything is loaded
		bool loaded = false;
		for ( auto &sound : Sounds ) {
			loaded = sound->IsInitialized();
			if ( !loaded ) {
				break;
			}
		}

		for ( auto &texture : Textures ) {
			loaded = texture->IsInitialized();
			if ( !loaded ) {
				break;
			}
		}

		return loaded;
	}

	return false;
}//LoadContent()

bool MapLoadingState::Impl::LoadMap( GameManager *manager ) {
	auto map = manager->GetGameplayManager();
	
	if ( !mapLoadingStarted ) {
		//Load Map
		map->Initialize( manager );
		map->Load( manager->GetStyleManager()->GetLevelFilename( levelIndex ) );
		map->SetLevelIndex( levelIndex );

		if ( isResumed ) {
			auto p = map->GetPlayer();
			p->TempPoints = p->Points = points;
			p->TempHealth = p->Health = health;
			isResumed = false;
		}

		mapLoadingStarted = true;
	}

	//Return true if map is loaded
	return map->IsLoaded();
}//LoadMap()

MapLoadingState::MapLoadingState( int levelIndex, std::uint8_t health, std::uint32_t points ) :
	pImpl( new Impl( levelIndex, health, points ) ) {
}

MapLoadingState::MapLoadingState( std::uint8_t levelIndex ) :
	pImpl( new Impl( levelIndex ) ) {
}//Ctor()

MapLoadingState::~MapLoadingState() {
	pImpl = nullptr;
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( MapLoadingState );

void MapLoadingState::Load( GameManager *manager ) {
	GameState::Load( manager );

	pImpl->contentLoadingStarted	= false;
	pImpl->mapLoadingStarted		= false;

	auto graphics	= manager->GetGraphicsManager();
	auto content	= manager->GetMenuContent();

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

	//Initialize Common Data
	pImpl->Initialize( manager );
}//Load()

void MapLoadingState::Unload() {
}//Unload()

void MapLoadingState::Pause() {
}//Pause()

void MapLoadingState::Resume() {
	pImpl->Initialize( gameManager );
}//Resume()

void MapLoadingState::Update( float elapsedTime, float totalTime )
{
	if ( pImpl->LoadContent( gameManager ) ) {
		auto style	= gameManager->GetStyleManager();
		auto map	= gameManager->GetGameplayManager();
		auto hs		= gameManager->GetHighscore();

		if ( map->IsGameQuit() ) {
			if ( map->IsGameLost() || map->IsGameWon() ) {
				hs->AddHighscore( hs->GetUserName(), map->GetPlayer()->Points, 5 );
			}

			//Back to main menu
			map->UnInitialize();
			gameManager->GetGameplayContent()->Unload();
			gameManager->PopGameState();
			return;
		} else if ( map->IsGameLost() ) {
			hs->AddHighscore( hs->GetUserName(), map->GetPlayer()->Points, 5 );

			//Try Map Cycle again
			map->UnInitialize();
			pImpl->levelIndex = 0;
			pImpl->mapLoadingStarted = false;
		} else if ( map->IsGameWon() ) {
			hs->AddHighscore( hs->GetUserName(), map->GetPlayer()->Points, 5 );

			//Go to next Map
			map->Unload();
			pImpl->levelIndex++;
			pImpl->mapLoadingStarted = false;

			if ( pImpl->levelIndex >= style->GetLevelCount() ) {
				map->UnInitialize();
				gameManager->GetGameplayContent()->Unload();
				gameManager->PopGameState();
				gameManager->PushGameState( GameState::Create<HighscoreState>() );
				return;
			}
		}

		if ( pImpl->LoadMap( gameManager ) ) {
			//Start Game
			gameManager->PushGameState( GameState::Create<GameplayState>() );
		}
	}
}//Update()

void MapLoadingState::Draw( float elapsedTime, float totalTime ) {
	auto sprites = gameManager->GetSpriteBatch();

	sprites->Begin();
	pImpl->objLogo->Draw( sprites );
	pImpl->segoeUIsemi20->DrawString( sprites, L"LOADING DATA", XMLoadFloat2( &pImpl->texPos ), Colors::White, 0.0f, g_XMZero, 1.5f );
	sprites->End();
}//Draw()
