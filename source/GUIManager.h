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
#include "GameManager.h"

namespace BreakIt {
	namespace GUI {
		typedef std::function<void(void)> OnClicked;
		
		class GUIManager {
		public:
			explicit GUIManager();
			virtual ~GUIManager();
			UTILITY_CLASS_MOVE( GUIManager );

			float	GetGameTextWidth( const wchar_t *text );
			void	Initialize( const std::shared_ptr<WinGame::Graphics::Texture2D> &texture, const std::shared_ptr<DirectX::SpriteFont> &gameFont, const std::shared_ptr<DirectX::SpriteFont> &basicFont, bool widescreen );
			void	Resize( bool widescreen );

			void AddButton( float x, float y, const wchar_t *text, const OnClicked &clicked );
			void ClearButtons();

			void AddStaticText( float x, float y, DirectX::FXMVECTOR, const wchar_t *text );
			void ClearStaticText();

			void ClearAll();
			void Update( WinGame::Game::GameManager *gameManager );

			void Draw( DirectX::SpriteBatch *batch, bool drawSplitters = false );
			void DrawText( DirectX::SpriteBatch *batch, float r, float g, float b, float x, float y, const wchar_t *format, ... );
			void DrawGameText( DirectX::SpriteBatch *batch, float r, float g, float b, float x, float y, float size, const wchar_t *format, ... );
			void DrawLine( DirectX::SpriteBatch *batch, float x1, float y1, float x2, float y2, float width = 1.0f, DirectX::FXMVECTOR color = DirectX::Colors::White );
			void DrawRectangle( DirectX::SpriteBatch *batch, float x, float y, float width, float height, DirectX::FXMVECTOR color = DirectX::Colors::White );

		private:
			UTILITY_CLASS_COPY( GUIManager );
			UTILITY_CLASS_PIMPL();

		};//GUIManager class

	}//GUI namespace
}//BreakIt namespace
