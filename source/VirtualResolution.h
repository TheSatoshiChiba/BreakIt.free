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

#include "GraphicsManager.h"

namespace WinGame {
	namespace Graphics {
		class VirtualResolution {
		public:
			float Width;
			float Height;

			explicit VirtualResolution();
			VirtualResolution( float width, float height );

			bool				IsWidescreen() const;
			float				GetAspectRatio() const;
			DirectX::XMMATRIX	GetScaleMatrix( const GraphicsManager* graphics ) const;

			void				SetVirtualViewport( GraphicsManager* graphics );
			DirectX::XMVECTOR	Unproject( const GraphicsManager* graphics, DirectX::FXMVECTOR point ) const;

		private:
			CD3D11_VIEWPORT viewport;

		};//VirtualResolution class
	}//Graphics namespace
}//WinGame namespace
