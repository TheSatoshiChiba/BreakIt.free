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

#include "GameObject.h"
#include "Texture2D.h"

namespace BreakIt {
	namespace Objects {
		class IDrawableObject {
		public:
			explicit IDrawableObject() :
				Texture( nullptr ),
				RenderColor( 1.0f, 1.0f, 1.0f, 1.0f ),
				IsVisible( true ) {

				SourceRect.left		= 0;
				SourceRect.top		= 0;
				SourceRect.right	= 1;
				SourceRect.bottom	= 1;
			}//Ctor()

			virtual ~IDrawableObject() {
			}

			virtual void Draw( DirectX::SpriteBatch *batch ) = 0;

			bool				IsVisible;
			RECT				SourceRect;
			DirectX::XMFLOAT4	RenderColor;

			std::shared_ptr<WinGame::Graphics::Texture2D> Texture;

		};//IDrawableObject interface

	}//Objects namespace
}//BreakIt namespace
