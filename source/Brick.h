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
#include "Texture2D.h"
#include "SpriteBatch.h"

namespace BreakIt {
	namespace Objects {
		class Brick : public DrawableObject {
		public:
			explicit Brick();
			Brick( float x, float y, std::uint8_t health, const std::shared_ptr<WinGame::Graphics::Texture2D> &texture );
			virtual ~Brick();

			void DrawAnimated( DirectX::SpriteBatch *batch, const RECT &sourceRECT );

			static std::shared_ptr<Brick>	CreateBrickFromColor( std::uint8_t red, std::uint8_t green, std::uint8_t blue );
			static DirectX::XMVECTOR		RenderColorFromHealth( std::int8_t health );
			static bool						IsRemoveReady( const std::shared_ptr<Brick> &brick );
			
			std::uint8_t Health;
			std::int32_t Points;
		};//Brick class

	}//Objects namespace
}//BreakIt namespace
