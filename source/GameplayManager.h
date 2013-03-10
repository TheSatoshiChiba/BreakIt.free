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

#pragma once

#include "GameManager.h"
#include "BallManager.h"
#include "BrickManager.h"
#include "ItemManager.h"
#include "Player.h"
#include "SoundManager.h"

namespace BreakIt {
	namespace Objects {
		class GameplayManager {
		public:
			explicit GameplayManager();
			virtual ~GameplayManager();
			UTILITY_CLASS_MOVE( GameplayManager );
			
			bool	IsLoaded() const;
			bool	IsInitialized() const;
			bool	IsGameWon() const;
			bool	IsGameLost() const;
			bool	IsGameQuit() const;
			bool	IsGamePaused() const;
			int		GetLevelIndex() const;

			Player*			GetPlayer() const;
			ItemManager*	GetItems() const;
			BrickManager*	GetBricks() const;
			BallManager*	GetBalls() const;
			SoundManager*	GetSounds() const;

			void QuitGame();
			void Initialize( WinGame::Game::GameManager *manager );
			void UnInitialize();
			void Resize( bool widescreen );
			void Update( DirectX::FXMVECTOR playerDelta, float elapsedTime, float totalTime );
			void Draw( DirectX::SpriteBatch *batch );
			void SetPause( bool pause );
			void SetLevelIndex( int index );

			void Unload();
			void Load( const wchar_t *filename );

		private:
			UTILITY_CLASS_COPY( GameplayManager );
			UTILITY_CLASS_PIMPL();

		};//GameplayManager class
	}//Objects namespace
}//BreakIt namespace
