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

#include "pch.h"
#include "ContentManager.h"

using namespace WinGame::Content;
using namespace WinGame::Graphics;
using namespace WinGame::Audio;

class ContentManager::Impl {
public:
	Impl();

	std::map<std::wstring, std::shared_ptr<Texture2D>>				textureList;
	std::map<std::wstring, std::shared_ptr<Sound>>					soundList;
	std::map<std::wstring, std::shared_ptr<Music>>					musicList;
	std::map<std::wstring, std::shared_ptr<DirectX::SpriteFont>>	fontList;
};//ContentManager::Impl class

ContentManager::Impl::Impl() {
}//Ctor()

ContentManager::ContentManager() :
	pImpl( new Impl() ) {
}//Ctor()

ContentManager::~ContentManager() {
	pImpl = nullptr;
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( ContentManager );

std::shared_ptr<DirectX::SpriteFont> ContentManager::LoadFont( const GraphicsManager *graphics, const wchar_t *filename ) {
	if ( pImpl->fontList[filename] == nullptr ) {
		pImpl->fontList[filename] = graphics->CreateSpriteFont( filename );
	}

	return pImpl->fontList[filename];
}//LoadFont()

std::shared_ptr<Texture2D> ContentManager::LoadTexture2D( const GraphicsManager *graphics, const wchar_t *filename ) {
	if ( pImpl->textureList[filename] == nullptr ) {
		pImpl->textureList[filename] = graphics->CreateTexture2D( filename );
	}

	return pImpl->textureList[filename];
}//LoadTexture2D()

std::shared_ptr<Sound> ContentManager::LoadSound( const AudioManager *audio, const wchar_t *filename ) {
	if ( pImpl->soundList[filename] == nullptr ) {
		pImpl->soundList[filename] = audio->CreateSound( filename );
	}

	return pImpl->soundList[filename];
}//LoadSound()

std::shared_ptr<Music> ContentManager::LoadMusic( const AudioManager *audio, const wchar_t *filename ) {
	if ( pImpl->musicList[filename] == nullptr ) {
		pImpl->musicList[filename] = audio->CreateMusic( filename );
	}

	return pImpl->musicList[filename];
}//LoadMusic()

void ContentManager::Unload() {
	pImpl->textureList.clear();
	pImpl->soundList.clear();
	pImpl->musicList.clear();
	pImpl->fontList.clear();
}//Unload()

void ContentManager::UnloadTextures() {
	pImpl->textureList.clear();
}//UnloadTextures()

void ContentManager::UnloadSounds() {
	pImpl->soundList.clear();
}//UnloadSound()

void ContentManager::UnloadMusic() {
	pImpl->musicList.clear();
}//UnloadMusic()

void ContentManager::UnloadFonts() {
	pImpl->fontList.clear();
}//UnloadFonts()
