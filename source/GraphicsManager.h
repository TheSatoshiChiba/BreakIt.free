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
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "CommonStates.h"

namespace WinGame {
	namespace Graphics {
		class GraphicsManager {
		public:
			explicit GraphicsManager();
			virtual	~GraphicsManager();
			UTILITY_CLASS_MOVE( GraphicsManager );

			bool				IsInitialized() const;
			float				GetDPI() const;
			D3D_FEATURE_LEVEL	GetFeatureLevel() const;
			float				GetViewportWidth() const;
			float				GetViewportHeight() const;
			float				GetAspectRatio() const;
			bool				IsWidescreen() const;

			const DirectX::CommonStates * GetCommonStates() const;

			void Initialize( Windows::UI::Core::CoreWindow ^window, float dpi );
			void Resize();
			void Clear();
			void Present();

			void ResetViewport();
			void SetViewport( const CD3D11_VIEWPORT &viewport );
			void SetClearColor( DirectX::FXMVECTOR clearColor );
			void SetDPI( float dpi );

			std::shared_ptr<Texture2D>				CreateTexture2D( const wchar_t *filename ) const;
			std::shared_ptr<DirectX::SpriteBatch>	CreateSpriteBatch() const;
			std::shared_ptr<DirectX::SpriteFont>	CreateSpriteFont( const wchar_t *filename ) const;

		private:
			UTILITY_CLASS_COPY( GraphicsManager );
			UTILITY_CLASS_PIMPL();

		};//GraphicsManager class

	}//Graphics namespace
}//BreakIt namespace
