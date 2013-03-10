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

#include "Texture2D.h"

namespace BreakIt {
	namespace GUI {
		class GUIObject {
		public:
			explicit GUIObject( float x, float y, float width, float height, const std::shared_ptr<WinGame::Graphics::Texture2D> &texture ) :
				Position( x, y ),
				Size( width, height ),
				Origin( 0.0f, 0.0f ),
				RenderColor( 1.0f, 1.0f, 1.0f, 1.0f ),
				Texture( texture ),
				Rotation( 0.0f ),
				IsVisible( true ) {

				SourceRect.top		= 0;
				SourceRect.left		= 0;
				SourceRect.right	= Utility::ftoi( width );
				SourceRect.bottom	= Utility::ftoi( height );
			}//Ctor()

			virtual ~GUIObject() {
			}//Dtor()

			virtual void Draw( DirectX::SpriteBatch *batch ) {
				if ( !Texture || !Texture->IsInitialized() || !IsVisible ) {
					return;
				}

				RECT dest = {
					Utility::ftoi( Position.x ),
					Utility::ftoi( Position.y ),
					Utility::ftoi( Position.x + Size.x ),
					Utility::ftoi( Position.y + Size.y )
				};

				batch->Draw( Texture->GetResourceView(), dest, &SourceRect, DirectX::XMLoadFloat4( &RenderColor ), Rotation, Origin );
			}//Draw()

			bool	IsVisible;
			RECT	SourceRect;
			float	Rotation;

			DirectX::XMFLOAT2 Position;
			DirectX::XMFLOAT2 Size;
			DirectX::XMFLOAT2 Origin;
			DirectX::XMFLOAT4 RenderColor;

			std::shared_ptr<WinGame::Graphics::Texture2D> Texture;
		};//GUIObject class

	}//GUI namespace
}//BreakIt namespace
