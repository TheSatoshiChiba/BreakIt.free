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
#include "Diamond.h"
#include "Globals.h"

using namespace BreakIt;
using namespace BreakIt::Objects;
using namespace DirectX;

Diamond::Diamond( float x, float y, const std::shared_ptr<WinGame::Graphics::Texture2D> &texture ) :
	Item( x, y, texture ) {

	SourceRect.left		= 0;
	SourceRect.top		= 136;
	SourceRect.right	= SourceRect.left + Utility::ftoi( ItemTextureWidth );
	SourceRect.bottom	= SourceRect.top + Utility::ftoi( ItemTextureHeight );

	XMStoreFloat4( &RenderColor, Colors::White );
}//Ctor()

Diamond::~Diamond() {
}//Dtor()

void Diamond::ApplyEffect( Player *player, BallManager *balls ) {
	player->Points += 1000;
}//ApplyEffect()
