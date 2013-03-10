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

#include "DrawableObject.h"
#include "Laser.h"
#include "SoundManager.h"

namespace BreakIt {
	namespace Objects {
		class BrickManager;
		class ItemManager;

		class Player : public DrawableObject {
		public:
			explicit Player( float x, float y, const std::shared_ptr<WinGame::Graphics::Texture2D> &texture );
			virtual ~Player();

			void			Update( float elapsedTime, float totalTime, SoundManager *sound, BrickManager *bricks, ItemManager *items );
			virtual void	Draw( DirectX::SpriteBatch *batch ) override;
			
			void Clamp( DirectX::FXMVECTOR leftBorderMax, DirectX::FXMVECTOR rightBorderMin );
			void Move( DirectX::FXMVECTOR delta );
			void Grow();
			void Shrink();
			void ResetGrowth();
			void ActivateLaser();
			void ResetLaser();

			bool	IsLaserActive() const;
			int		GetLaserCount() const;

			std::uint8_t Health;
			std::uint8_t TempHealth;

			std::uint32_t Points;
			std::uint32_t TempPoints;

		private:
			std::uint8_t	growSize;
			LONG			leftOffset;
			float			deltaX;

			float	laserTime;
			bool	laserActivated;
			int		laserCount;

			std::vector<std::unique_ptr<Laser>> laserShots;
			std::unique_ptr<DrawableObject>		canon;
		};//Player class

	}//Objects namespace
}//BreakIt namespace
