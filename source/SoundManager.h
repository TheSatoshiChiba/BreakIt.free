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

#include "Sound.h"
#include "GameManager.h"
#include "Globals.h"

namespace BreakIt {
	namespace Objects {
		enum class SOUND_FILE : std::uint8_t {
			UNKNOWN = 0x00U,
			SIDE_HIT,
			BALL_LOST,
			PLAYER_HIT,
			BRICK_HIT,
			BRICK_BREAK,
			GAME_WON,
			GAME_LOST,
			POINTS_ADD,
			HEALTH_ADD,
			POSITIVE_ITEM,
			NEGATIVE_ITEM,
			LASER_SHOT
		};//SOUND_FILE enum class

		class SoundManager {
		public:
			explicit SoundManager();
			virtual ~SoundManager();
			UTILITY_CLASS_MOVE( SoundManager );

			void Initialize( WinGame::Game::GameManager *manager );
			void PlaySound( SOUND_FILE file, float volume = 1.0f );
			void PlayItemSound( ITEM_TYPES type, float volume = 1.0f );

		private:
			UTILITY_CLASS_COPY( SoundManager );
			UTILITY_CLASS_PIMPL();

		};//SoundManager class
	}//Objects namespace
}//BreakIt namespace
