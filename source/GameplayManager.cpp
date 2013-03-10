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

#include "pch.h"
#include "GameplayManager.h"
#include "Globals.h"

using namespace WinGame;
using namespace WinGame::Audio;
using namespace WinGame::Content;
using namespace WinGame::Graphics;
using namespace WinGame::Game;
using namespace BreakIt;
using namespace BreakIt::Objects;
using namespace BreakIt::Styles;
using namespace DirectX;

class GameplayManager::Impl {
public:
	Impl();

	bool isWidescreen;
	bool isLoaded;
	bool isInitialized;
	bool isGameWon;
	bool isGameLost;
	bool isGameQuit;
	bool isGamePaused;

	int levelNum;

	//Resources
	std::shared_ptr<Texture2D> styleTexture;

	//Objects
	std::unique_ptr<BallManager>	ballManager;
	std::unique_ptr<ItemManager>	itemManager;
	std::unique_ptr<BrickManager>	brickManager;
	std::unique_ptr<SoundManager>	soundManager;
	std::unique_ptr<Player>			player;

	RECT pipeRECT;
	RECT deathRECT;

	//Methods
	void ResetPlayer();
};//GameplayManager::Impl class

GameplayManager::Impl::Impl() :
	isWidescreen( false ),
	isLoaded( false ),
	isInitialized( false ),
	isGameWon( false ),
	isGameLost( false ),
	isGameQuit( false ),
	isGamePaused( false ),
	ballManager( new BallManager() ),
	itemManager( new ItemManager() ),
	brickManager( new BrickManager() ),
	soundManager( new SoundManager() ),
	player( nullptr ) {

	pipeRECT.left	= 0;
	pipeRECT.top	= 170;
	pipeRECT.right	= pipeRECT.left + 71;
	pipeRECT.bottom = pipeRECT.top + 16;

	deathRECT.left		= 34;
	deathRECT.top		= 187;
	deathRECT.right		= deathRECT.left + 32;
	deathRECT.bottom	= deathRECT.top + 32;
}//Ctor()

void GameplayManager::Impl::ResetPlayer() {
	float screenCenterX = GameFieldWidth * 0.5f + SplitterWidth;

	if ( isWidescreen ) {
		screenCenterX += SidebarWidth;
	}

	player->Position.x = screenCenterX - player->HitBoxSize.x * 0.5f;
}//ResetPlayer()

GameplayManager::GameplayManager() :
	pImpl( new Impl() ) {
}//Ctor()

GameplayManager::~GameplayManager() {
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( GameplayManager );

bool GameplayManager::IsLoaded() const {
	return pImpl->isLoaded;
}//IsLoaded()

bool GameplayManager::IsInitialized() const {
	return pImpl->isInitialized;
}//IsInitialized()

bool GameplayManager::IsGameLost() const {
	return pImpl->isGameLost;
}//IsGameLost()

bool GameplayManager::IsGameWon() const {
	return pImpl->isGameWon;
}//IsGameWon()

bool GameplayManager::IsGameQuit() const {
	return pImpl->isGameQuit;
}//IsGameQuit()

bool GameplayManager::IsGamePaused() const {
	return pImpl->isGamePaused;
}//IsGamePaused()

Player* GameplayManager::GetPlayer() const {
	return pImpl->player.get();
}//GetPlayer()

ItemManager* GameplayManager::GetItems() const {
	return pImpl->itemManager.get();
}//GetCoins()

BrickManager* GameplayManager::GetBricks() const {
	return pImpl->brickManager.get();
}//GetBricks()

BallManager* GameplayManager::GetBalls() const {
	return pImpl->ballManager.get();
}//GetBalls()

SoundManager* GameplayManager::GetSounds() const {
	return pImpl->soundManager.get();
}//GetSounds()

int GameplayManager::GetLevelIndex() const {
	return pImpl->levelNum;
}//GetLevelIndex()

void GameplayManager::SetLevelIndex( int index ) {
	pImpl->levelNum = index;
}//SetLevelIndex()

void GameplayManager::Initialize( GameManager *manager ) {
	if ( pImpl->isInitialized ) {
		return;
	}

	auto content	= manager->GetGameplayContent();
	auto graphics	= manager->GetGraphicsManager();
	auto style		= manager->GetStyleManager();

	float screenCenterX = GameFieldWidth * 0.5f + SplitterWidth;
	
	if ( graphics->GetAspectRatio() <= 1.34f ) {
		pImpl->isWidescreen = false;
	} else {
		pImpl->isWidescreen = true;
		screenCenterX += SidebarWidth;
	}

	//Set Textures
	pImpl->styleTexture = content->LoadTexture2D( graphics, style->GetStyleTexture() );
	
	//Init Game Objects
	pImpl->ballManager->Initialize( pImpl->styleTexture, pImpl->isWidescreen );
	pImpl->brickManager->Initialize( pImpl->styleTexture, pImpl->isWidescreen );
	pImpl->itemManager->Initialize( pImpl->styleTexture, pImpl->isWidescreen );
	pImpl->soundManager->Initialize( manager );

	if ( pImpl->player ) {
		pImpl->player = nullptr;
	}

	pImpl->player = std::make_unique<Player>(
		0.0f,
		GameFieldHeight - PlayerTextureHeight - 42.0f,
		pImpl->styleTexture
		);

	pImpl->player->Position.x	= screenCenterX - pImpl->player->HitBoxSize.x * 0.5f;
	pImpl->player->Health		= PlayerStartHealth;
	pImpl->player->Points		= 0;
	pImpl->player->TempPoints	= 0;
	pImpl->player->TempHealth	= PlayerStartHealth;

	//Set Default States
	pImpl->isLoaded			= false;
	pImpl->isInitialized	= true;
	pImpl->isGameLost		= false;
	pImpl->isGameWon		= false;
	pImpl->isGameQuit		= false;
	pImpl->isGamePaused		= false;
}//Initialize()

void GameplayManager::UnInitialize() {
	Unload();
	pImpl->isInitialized = false;
}//Uninitialize()

void GameplayManager::Resize( bool widescreen ) {
	if ( !pImpl->isInitialized ) {
		return;
	}

	pImpl->isGamePaused = true;
	if ( pImpl->isWidescreen != widescreen ) {
		pImpl->ballManager->Resize( widescreen );
		pImpl->brickManager->Resize( widescreen );
		pImpl->itemManager->Resize( widescreen );

		if ( pImpl->isWidescreen && !widescreen ) {
			pImpl->player->Position.x -= SidebarWidth;
		} else if ( !pImpl->isWidescreen && widescreen ) {
			pImpl->player->Position.x += SidebarWidth;
		}

		pImpl->isWidescreen = widescreen;
	}
}//Resize()

void GameplayManager::QuitGame() {
	pImpl->isGameQuit = true;
}//QuitGame()

void GameplayManager::Update( FXMVECTOR playerDelta, float elapsedTime, float totalTime ) {
	if ( !pImpl->isInitialized || !pImpl->isLoaded || pImpl->isGameQuit ) {
		return;
	}

	auto items	= pImpl->itemManager.get();
	auto bricks = pImpl->brickManager.get();

	//Animate Stuff
	items->Animate( elapsedTime );
	bricks->Animate( elapsedTime );

	if ( pImpl->isGameLost || pImpl->isGamePaused || pImpl->isGameWon ) {
		return;
	}

	//Update Stuff
	auto sounds = pImpl->soundManager.get();
	auto player = pImpl->player.get();
	auto balls	= pImpl->ballManager.get();

	player->Update( elapsedTime, totalTime, sounds, bricks, items );
	player->Move( playerDelta );
	balls->Update( player, sounds, elapsedTime, totalTime );
	balls->CheckCollision( player, bricks, items, sounds );
	items->Update( player, balls, sounds, elapsedTime, totalTime );

	//Set new States
	if ( player->Health == 0 ) {
		sounds->PlaySound( SOUND_FILE::GAME_LOST );
		pImpl->isGameLost = true;
	} else {
		if ( bricks->GetCount() == 0 ) {
			sounds->PlaySound( SOUND_FILE::GAME_WON );
			items->GiveAll( player, balls );
			pImpl->isGameWon = true;
		} else if ( balls->GetCount() == 0 ) {
			balls->AddBall();
			items->Clear();
			pImpl->ResetPlayer();
			pImpl->isGamePaused = true;
		}
	}
}//Update()

void GameplayManager::Draw( SpriteBatch *batch ) {
	if ( !pImpl->isInitialized ) {
		return;
	}

	if ( pImpl->isLoaded ) {
		pImpl->brickManager->Draw( batch );

		RECT dest;
		LONG startPos = pImpl->isWidescreen ? Utility::ftoi( SplitterWidth + SidebarWidth ) : Utility::ftoi( SplitterWidth );

		//Draw PIPE
		for ( int i = 0; i < GameFieldWidth / 72; ++i ) {
			dest.left	= startPos + 72 * i;
			dest.top	= Utility::ftoi( pImpl->player->Position.y ) + 16;
			dest.right	= dest.left + 72;
			dest.bottom	= dest.top + 16;

			batch->Draw( pImpl->styleTexture->GetResourceView(), dest, &pImpl->pipeRECT );
		}

		startPos = Utility::ftoi( pImpl->isWidescreen ? SidebarWidth + SplitterWidth : SplitterWidth ) - 8;

		//DRAW DeathZone
		for ( int i = 0; i < 23; ++i ) {
			dest.left	= startPos + i * 32;
			dest.top	= Utility::ftoi( GameFieldHeight ) - 32;
			dest.right	= dest.left + 32;
			dest.bottom = pImpl->isWidescreen ? 32 : 92;
			dest.bottom += dest.top;

			batch->Draw( pImpl->styleTexture->GetResourceView(), dest, &pImpl->deathRECT, XMVectorSet( 1.0f, 1.0f, 1.0f, 0.2f ) );
		}

		pImpl->player->Draw( batch );
		pImpl->ballManager->Draw( batch );
		pImpl->itemManager->Draw( batch );
	}
}//Draw()

void GameplayManager::SetPause( bool pause ) {
	pImpl->isGamePaused = pause;
}//Pause()

void GameplayManager::Unload() {
	if ( pImpl->isLoaded ) {
		pImpl->brickManager->Clear();
		pImpl->ballManager->Clear();
		pImpl->itemManager->Clear();
		pImpl->ResetPlayer();
		pImpl->player->ResetGrowth();
		pImpl->player->ResetLaser();
		
		pImpl->isLoaded = false;

		pImpl->isGameLost = false;
		pImpl->isGameWon = false;
		pImpl->isGameQuit = false;
		pImpl->isGamePaused = false;
	}
}//Unload()

void GameplayManager::Load( const wchar_t *filename ) {
	if ( !pImpl->isInitialized ) {
		return;
	}

	if ( pImpl->isLoaded ) {
		Unload();
	}

	auto folder			= Windows::ApplicationModel::Package::Current->InstalledLocation;
	auto getFileTask	= create_task( folder->GetFileAsync( ref new Platform::String( filename ) ) );

	auto readDataTask = getFileTask.then( [this]( StorageFile ^file ) {
		return file->OpenReadAsync();

	} ).then( [this]( Windows::Storage::Streams::IRandomAccessStreamWithContentType ^stream ) {
		return Windows::Graphics::Imaging::BitmapDecoder::CreateAsync( Windows::Graphics::Imaging::BitmapDecoder::BmpDecoderId, stream );

	} ).then( [this]( Windows::Graphics::Imaging::BitmapDecoder ^decoder ) {
		return decoder->GetPixelDataAsync();

	} ).then( [this]( Windows::Graphics::Imaging::PixelDataProvider ^pixelProvider ) {
		auto data = pixelProvider->DetachPixelData();

		float texOffset		= ( ItemTextureWidth - ItemWidth ) * 0.5f;
		float startPosX		= 0.0f;
		float startPosY		= -texOffset;
		float baseStartX	= 0.0f;

		std::uint8_t CountX = 0;
		std::uint8_t CountY = 0;

		if ( !pImpl->isWidescreen ) {
			baseStartX = SplitterWidth - texOffset;
		} else {
			baseStartX = SplitterWidth + SidebarWidth - texOffset;
		}

		startPosX = baseStartX;
		for ( decltype( data->Length ) i = 0; i < data->Length; i += 4 ) {
			pImpl->brickManager->AddBrick( startPosX, startPosY, data[i], data[i+1], data[i+2] );

			++CountX;
			startPosX += ItemWidth;

			if ( CountX >= BricksWide ) {
				CountX = 0;
				++CountY;
				
				startPosX = baseStartX;
				startPosY += ItemHeight;

				if ( CountY >= BricksHeigh ) {
					break;
				}
			}
		}

		pImpl->ResetPlayer();
		pImpl->player->ResetGrowth();
		pImpl->player->ResetLaser();
		pImpl->player->TempPoints = pImpl->player->Points;
		pImpl->player->TempHealth = pImpl->player->Health;
		pImpl->ballManager->AddBall();

		pImpl->isGamePaused = true;
		pImpl->isLoaded = true;
	} );
}//Load()
