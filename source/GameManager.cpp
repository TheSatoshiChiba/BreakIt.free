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
#include "GameManager.h"
#include "SnappedState.h"
#include "GameplayManager.h"
#include "BasicStyle.h"

using namespace WinGame;
using namespace WinGame::Game;
using namespace WinGame::Graphics;
using namespace WinGame::Input;
using namespace WinGame::Content;
using namespace WinGame::Audio;
using namespace BreakIt;
using namespace BreakIt::Styles;
using namespace BreakIt::Objects;

class GameManager::Impl {
public:
	Impl();

	bool			isActive;
	bool			isSnapped;
	std::int32_t	fps;
	std::int32_t	fpsCounter;
	float			fpsTime;

	std::vector<std::unique_ptr<GameState>> gameStates;
	std::unique_ptr<GraphicsManager>		graphicsManager;
	std::unique_ptr<InputManager>			inputManager;
	std::unique_ptr<ContentManager>			menuContent;
	std::unique_ptr<ContentManager>			gameContent;
	std::unique_ptr<AudioManager>			audioManager;
	std::shared_ptr<DirectX::SpriteBatch>	spriteBatch;
	std::unique_ptr<VirtualResolution>		virtualResolution;
	std::unique_ptr<IStyle>					styleManager;
	std::unique_ptr<GameplayManager>		levelManager;
	std::unique_ptr<Utility::BasicTimer>	basicTimer;

	std::unique_ptr<ddHighscore> highscore;

	void UpdateFPS();
};//GameManager::Impl class

GameManager::Impl::Impl() :
	isActive( true ),
	isSnapped( false ) {
}//Ctor()

void GameManager::Impl::UpdateFPS() {
	fpsCounter++;
	fpsTime += basicTimer->GetDeltaTime();

	if ( fpsTime >= 1.0f ) {
		fpsTime		-= 1.0f;
		fps			= fpsCounter;
		fpsCounter	= 0;
	}
}//UpdateFPS()

GameManager::GameManager() :
	pImpl( new Impl() ),
	isResumed( false ) {
}//Ctor()

GameManager::~GameManager() {
	pImpl = nullptr;
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( GameManager );

GraphicsManager* GameManager::GetGraphicsManager() const {
	return pImpl->graphicsManager.get();
}//GetGraphicsManager()

InputManager* GameManager::GetInputManager() const {
	return pImpl->inputManager.get();
}//GetInputManager()

ContentManager* GameManager::GetMenuContent() const {
	return pImpl->menuContent.get();
}//GetMenuContent()

ContentManager* GameManager::GetGameplayContent() const {
	return pImpl->gameContent.get();
}//GetGameplayContent()

AudioManager* GameManager::GetAudioManager() const {
	return pImpl->audioManager.get();
}//GetAudioManager()

DirectX::SpriteBatch* GameManager::GetSpriteBatch() const {
	return pImpl->spriteBatch.get();
}//SpriteManager()

VirtualResolution* GameManager::GetVirtualResolution() const {
	return pImpl->virtualResolution.get();
}//GetVirtualResolution()

IStyle* GameManager::GetStyleManager() const {
	return pImpl->styleManager.get();
}//GetStyleManager()

GameplayManager* GameManager::GetGameplayManager() const {
	return pImpl->levelManager.get();
}//GetLevelManager()

int GameManager::GetFPS() const {
	return pImpl->fps;
}//GetFPS()

ddHighscore* GameManager::GetHighscore() {
	return pImpl->highscore.get();
}

const ddHighscore* GameManager::GetHighscore() const {
	return pImpl->highscore.get();
}

void GameManager::SetStyleManager( std::unique_ptr<IStyle> &&manager ) {
	if ( pImpl->styleManager ) {
		pImpl->styleManager = nullptr;
	}

	pImpl->styleManager = std::move( manager );
}//SetStyleManager()

void GameManager::Initialize( Windows::UI::Core::CoreWindow ^gameWindow, float width, float height ) {
	pImpl->fpsTime	= 0.0f;
	pImpl->fps		= pImpl->fpsCounter = 0;

	pImpl->highscore		= std::make_unique<ddHighscore>();
	pImpl->basicTimer		= std::make_unique<Utility::BasicTimer>();
	pImpl->inputManager		= std::make_unique<InputManager>();
	pImpl->graphicsManager	= std::make_unique<GraphicsManager>();
	pImpl->audioManager		= std::make_unique<AudioManager>();

	pImpl->highscore->ReadHighscore();
	pImpl->menuContent = std::make_unique<ContentManager>();
	pImpl->gameContent = std::make_unique<ContentManager>();
	pImpl->menuContent->Unload();
	pImpl->gameContent->Unload();

	pImpl->virtualResolution	= std::make_unique<VirtualResolution>( width, height );
	pImpl->levelManager			= std::make_unique<GameplayManager>();
	
	pImpl->inputManager->Initialize();
	pImpl->audioManager->Initialize();
	pImpl->graphicsManager->Initialize( gameWindow, Windows::Graphics::Display::DisplayProperties::LogicalDpi );
	pImpl->spriteBatch = pImpl->graphicsManager->CreateSpriteBatch();
	//pImpl->levelManager->Initialize();
	//pImpl->levelManager->Resize(pImpl->graphicsManager->GetAspectRatio());
}//Initialize()

void GameManager::Resize() {
	pImpl->graphicsManager->Resize();

#if _DEBUG
	Utility::WriteDebugMessage( L"Aspect Ratio: %f\n", pImpl->graphicsManager->GetAspectRatio() );
	Utility::WriteDebugMessage( L"DPI: %f\n", pImpl->graphicsManager->GetDPI() );
#endif

	if ( pImpl->graphicsManager->GetAspectRatio() < 1.2f ) {
		if ( !pImpl->isSnapped ) {
			pImpl->isSnapped = true;
			PushGameState( GameState::Create<BreakIt::GameStates::SnappedState>() );
		}
	} else {
		if ( pImpl->isSnapped ) {
			pImpl->isSnapped = false;
			PopGameState();
		} else {
			if ( !pImpl->gameStates.empty() ) {
				pImpl->gameStates.back()->Pause();
				pImpl->gameStates.back()->Resume();
			}
		}
	}

	pImpl->basicTimer->Reset();
}//Resize()

bool GameManager::Run( bool isWindowVisible ) { 
	if ( isWindowVisible ) {
		if ( !pImpl->isActive ) {
			pImpl->isActive = true;
			pImpl->basicTimer->Reset();
		}

		if ( !pImpl->gameStates.empty() ) {
			//Update Timer and Input
			pImpl->basicTimer->Update();
			Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents( Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent );
			pImpl->UpdateFPS();
			
			//Update and Draw States
			pImpl->gameStates.back()->Update( pImpl->basicTimer->GetDeltaTime(), pImpl->basicTimer->GetTotalTime() );
			if ( pImpl->gameStates.empty() ) {
				return false;
			}

			if ( !pImpl->graphicsManager->IsInitialized() ) {
				return false;
			}

			pImpl->graphicsManager->Clear();
			pImpl->gameStates.back()->Draw( pImpl->basicTimer->GetDeltaTime(), pImpl->basicTimer->GetTotalTime() );
			pImpl->graphicsManager->Present();

			//Update Input values
			pImpl->inputManager->Update();
			return true;
		}
	} else {
		pImpl->isActive = false;
		Windows::UI::Core::CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents( Windows::UI::Core::CoreProcessEventsOption::ProcessOneAndAllPending );
		return true;
	}

	return false;
}//Run()

void GameManager::Suspend() {
	pImpl->audioManager->Suspend();

	int index = pImpl->levelManager->GetLevelIndex();

	//Save Highscore
	pImpl->highscore->SaveHighscore();

	if ( index < 0 || pImpl->levelManager->IsGameLost() ) {
		return;
	}

	if ( pImpl->levelManager->IsGameWon() ) {
		++index;
	}

	//Save Player State
	auto settings	= Windows::Storage::ApplicationData::Current->RoamingSettings;
	auto folder		= Windows::Storage::ApplicationData::Current->RoamingFolder;
	auto container	= settings->CreateContainer( "BreakItResume", ApplicationDataCreateDisposition::Always );

	if ( settings->Containers->HasKey( "BreakItResume" ) ) {
		auto values = settings->Containers->Lookup( "BreakItResume" )->Values;

		values->Clear();
		values->Insert( "levelIndex", index );
		values->Insert( "playerPoints", pImpl->levelManager->GetPlayer()->TempPoints );
		values->Insert( "playerHealth", pImpl->levelManager->GetPlayer()->TempHealth );
	}
}//Suspend()

void GameManager::Resume() {
	pImpl->audioManager->Resume();

	auto settings	= Windows::Storage::ApplicationData::Current->RoamingSettings;
	auto folder		= Windows::Storage::ApplicationData::Current->RoamingFolder;

	levelIndex = 0;
	tempPoints = 0;
	tempHealth = 0;
	bool allOK = false;

	auto hasContainer = settings->Containers->HasKey( "BreakItResume" );

	if ( hasContainer ) {
		auto values = settings->Containers->Lookup( "BreakItResume" )->Values;

		if ( values->HasKey( "levelIndex" ) ) {
			levelIndex = safe_cast<int>( values->Lookup( "levelIndex" ) );
			
			if ( levelIndex >= 0 ) {
				if ( values->HasKey( "playerPoints" ) && values->HasKey( "playerHealth" ) ) {
					tempPoints = safe_cast<std::uint32_t>( values->Lookup( "playerPoints" ) );
					tempHealth = safe_cast<std::uint8_t>( values->Lookup( "playerHealth" ) );

					allOK = true;
				}
			}
		}

		values->Clear();
		values = nullptr;
		settings->DeleteContainer( "BreakItResume" );
	}

	if ( allOK && isResumed ) {
		//NOTE: Nothing to do here it seems... ( Bad Design )
	} else {
		isResumed = false;
	}
}//Resume()

void GameManager::PushGameState( std::unique_ptr<GameState> &&gameState ) {
	if ( !pImpl->gameStates.empty() ) {
		pImpl->gameStates.back()->Pause();
	}

	pImpl->gameStates.push_back( move( gameState ) );
	pImpl->gameStates.back()->Load( this );
}//PushGameState()

void GameManager::PopGameState() {
	if ( !pImpl->gameStates.empty() ) {
		pImpl->gameStates.back()->Unload();
		pImpl->gameStates.pop_back();

		if ( pImpl->gameStates.empty() ) {
			return;
		}

		pImpl->gameStates.back()->Resume();
	}
}//PopGameState()

void GameManager::ChangeGameState( std::unique_ptr<GameState> &&gameState ) {
	if ( !pImpl->gameStates.empty() ) {
		pImpl->gameStates.back()->Unload();
		pImpl->gameStates.pop_back();
	}

	pImpl->gameStates.push_back( move( gameState ) );
	pImpl->gameStates.back()->Load( this );
}//ChangeGameState()
