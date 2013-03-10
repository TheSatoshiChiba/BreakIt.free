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
#include "Item.h"
#include "Globals.h"

using namespace BreakIt;
using namespace BreakIt::Objects;
using namespace DirectX;

Item::Item( float x, float y, const std::shared_ptr<WinGame::Graphics::Texture2D> &texture ) :
	DrawableObject( x, y, ItemTextureWidth, ItemTextureHeight, texture ),
	Acceleration( 0.0f, 0.0f ),
	Velocity( 0.0f, MinimumItemSpeed ) {

	HitBoxOffset = XMFLOAT2(
		( ItemTextureWidth - ItemWidth ) * 0.5f,
		( ItemTextureHeight - ItemHeight ) * 0.5f
		);
	HitBoxSize = XMFLOAT2( ItemWidth, ItemHeight );
}//Ctor()

Item::~Item() {
}//Dtor()

void Item::Update( float elapsedTime, float totalTime ) {
	auto acc	= XMLoadFloat2( &Acceleration );
	auto vel	= XMLoadFloat2( &Velocity );
	auto pos	= XMLoadFloat2( &Position );
	auto oldVel = vel;

	vel = XMVectorAdd( vel, XMVectorScale( acc, elapsedTime ) );
	pos = XMVectorAdd( pos, XMVectorScale( XMVectorAdd( oldVel, vel ), elapsedTime ) );

	XMStoreFloat2( &Velocity, vel );
	XMStoreFloat2( &Position, pos );
}//Update()

void Item::DrawAnimated( SpriteBatch *batch, const RECT &sourceRECT ) {
	if ( !Texture || !Texture->IsInitialized() || !IsVisible ) {
		return;
	}

	RECT dest = {
		Utility::ftoi( Position.x ),
		Utility::ftoi( Position.y ),
		Utility::ftoi( Position.x + Size.x ),
		Utility::ftoi( Position.y + Size.y )
	};

	batch->Draw( Texture->GetResourceView(), dest, &sourceRECT, XMLoadFloat4( &RenderColor ), XMConvertToRadians( Rotation ), Origin );
}//DrawAnimated()

bool Item::IsRemoveReady( const std::unique_ptr<Item> &item ) {
	return !item->IsVisible;
}//IsRemoveReady()
