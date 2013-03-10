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

#include "ItemManager.h"
#include "Ball.h"
#include "Player.h"
#include "SoundManager.h"
#include "BrickManager.h"
#include "Globals.h"

namespace BreakIt {
	namespace Objects {
		class BallManager {
		public:
			explicit BallManager();
			virtual ~BallManager();
			UTILITY_CLASS_MOVE( BallManager );

			std::uint32_t GetCount() const;

			void Initialize( const std::shared_ptr<WinGame::Graphics::Texture2D> &texture, bool widescreen );
			void AddBall( float x, float y );
			void AddBall();
			void SplitBall();
			void Update( Player *player, SoundManager *sounds, float elapsedTime, float totalTime );
			void CheckCollision( Player *player, BrickManager *bricks, ItemManager *items, SoundManager *sounds );
			void Draw( DirectX::SpriteBatch *batch );
			void DrawStatic( DirectX::SpriteBatch *batch, float x, float y );
			void Clear();
			void Resize( bool widescreen );
			void SetPowerLevel( std::uint8_t power );
			void SetVisibility( bool visible );
			void ActivateWall();

			bool	IsInvisible() const;
			bool	IsBouncy() const;
			bool	IsPowerful() const;
			int		GetVisibleTime() const;
			int		GetPowerTime() const;
			bool	IsWallActive() const;
			int		GetWallTime() const;

		private:
			UTILITY_CLASS_COPY( BallManager );
			UTILITY_CLASS_PIMPL();

		};//BallManager class

	}//Objects namespace
}//BreakIt namespace
