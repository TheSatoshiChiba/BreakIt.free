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

#include "Brick.h"
#include "Ball.h"
#include "Sound.h"
#include "Player.h"
#include "ItemManager.h"

namespace BreakIt {
	namespace Objects {	
		class Laser;

		class BrickManager {
		public:
			explicit BrickManager();
			virtual ~BrickManager();
			UTILITY_CLASS_MOVE( BrickManager );

			std::uint32_t GetCount() const;

			void Initialize( const std::shared_ptr<WinGame::Graphics::Texture2D> &texture, bool widescreen );
			void Animate( float elapsedTime );
			void Draw( DirectX::SpriteBatch *batch );
			void DrawStatic( DirectX::SpriteBatch *batch, float x, float y, std::uint8_t health );
			void Clear();
			void Resize( bool widescreen );
			void AddBrick( float x, float y, std::uint8_t red, std::uint8_t green, std::uint8_t blue );
			void CheckCollision(Ball *ball, Player *player, SoundManager *sounds, ItemManager *items );
			void CheckCollision(Laser *shot, Player *player, SoundManager *sounds, ItemManager *items );

		private:
			UTILITY_CLASS_COPY( BrickManager );
			UTILITY_CLASS_PIMPL();

		};//BrickManager class

	}//Objects namespace
}//BreakIt namespace
