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

namespace BreakIt {
	namespace GameStates {
		class LoadingState : public WinGame::Game::GameState {
		public:
			explicit LoadingState();
			virtual ~LoadingState();
			UTILITY_CLASS_MOVE( LoadingState );

			virtual void Load( WinGame::Game::GameManager *manager ) override;
			virtual void Unload() override;
			virtual void Pause() override;
			virtual void Resume() override;
			virtual void Update( float elapsedTime, float totalTime ) override;
			virtual void Draw( float elapsedTime, float totalTime ) override;

		private:
			UTILITY_CLASS_COPY( LoadingState );
			UTILITY_CLASS_PIMPL();

		};//MenuLoadingState class

	}//GameStates namespace
}//BreakIt namespace
