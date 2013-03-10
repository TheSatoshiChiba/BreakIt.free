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
#include "AudioManager.h"

namespace WinGame {
	namespace Content {
		class ContentManager {
		public:
			explicit ContentManager();
			virtual ~ContentManager();

			UTILITY_CLASS_MOVE( ContentManager );

			std::shared_ptr<Graphics::Texture2D> LoadTexture2D(
				const Graphics::GraphicsManager *graphics, 
				const wchar_t *filename
				);

			std::shared_ptr<Audio::Sound> LoadSound(
				const Audio::AudioManager *audio,
				const wchar_t *filename
				);

			std::shared_ptr<Audio::Music> LoadMusic(
				const Audio::AudioManager *audio,
				const wchar_t *filename
				);

			std::shared_ptr<DirectX::SpriteFont> LoadFont(
				const Graphics::GraphicsManager *graphics,
				const wchar_t *filename
				);

			void Unload();
			void UnloadTextures();
			void UnloadSounds();
			void UnloadMusic();
			void UnloadFonts();

		private:
			UTILITY_CLASS_COPY( ContentManager );
			UTILITY_CLASS_PIMPL();

		};//ContentManager class

	}//Content namespace
}//WinGame namespace
