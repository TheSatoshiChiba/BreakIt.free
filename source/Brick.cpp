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
#include "Brick.h"
#include "Globals.h"

using namespace BreakIt;
using namespace BreakIt::Objects;
using namespace DirectX;
using namespace WinGame;
using namespace WinGame::Graphics;

Brick::Brick() :
	DrawableObject( 0.0f, 0.0f, ItemTextureWidth, ItemTextureHeight ),
	Health( 1 ),
	Points( 0 ) {

	HitBoxOffset	= XMFLOAT2( 4.0f, 4.0f );
	HitBoxSize		= XMFLOAT2( ItemWidth, ItemHeight );

	SourceRect.left		= 0;
	SourceRect.top		= 68;
	SourceRect.right	= SourceRect.left + Utility::ftoi( ItemTextureWidth );
	SourceRect.bottom	= SourceRect.top + Utility::ftoi( ItemTextureHeight );
}//Ctor()

Brick::Brick( float x, float y, std::uint8_t health, const std::shared_ptr<Texture2D> &texture ) :
	DrawableObject( x, y, ItemTextureWidth, ItemTextureHeight, texture ),
	Health( health > MaximumBrickHealth ? MaximumBrickHealth : health ),
	Points( 0 ) {

	HitBoxOffset	= XMFLOAT2( 4.0f, 4.0f );
	HitBoxSize		= XMFLOAT2( ItemWidth, ItemHeight );

	SourceRect.left		= 0;
	SourceRect.top		= 68;
	SourceRect.right	= SourceRect.left + Utility::ftoi( ItemTextureWidth );
	SourceRect.bottom	= SourceRect.top + Utility::ftoi( ItemTextureHeight );
}//Ctor()

Brick::~Brick() {
}//Dtor()

void Brick::DrawAnimated( SpriteBatch *batch, const RECT &sourceRECT ) {
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

std::shared_ptr<Brick> Brick::CreateBrickFromColor( std::uint8_t red, std::uint8_t green, std::uint8_t blue ) {
	auto result = std::make_shared<Brick>();

	if ( red == 0 && green == 255 && blue == 0 ) {
		result->Health = 1;
		result->Points = 1;

		XMStoreFloat4( &result->RenderColor, Brick::RenderColorFromHealth( 1 ) );

	} else if ( red == 0 && green == 0 && blue == 255 ) {
		result->Health = 2;
		result->Points = 2;

		XMStoreFloat4( &result->RenderColor, Brick::RenderColorFromHealth( 2 ) );

	} else if ( red == 255 && green == 255 && blue == 0 ) {
		result->Health = 3;
		result->Points = 3;

		XMStoreFloat4( &result->RenderColor, Brick::RenderColorFromHealth( 3 ) );

	} else if ( red == 255 && green == 0 && blue == 0 ) {
		result->Health = 4;
		result->Points = 4;

		XMStoreFloat4( &result->RenderColor, Brick::RenderColorFromHealth( 4 ) );

	} else if ( red == 255 && green == 255 && blue == 255 ) {
		result->Health = MaximumBrickHealth;
		result->Points = 5;

		XMStoreFloat4( &result->RenderColor, Brick::RenderColorFromHealth( 5 ) );

	} else {
		return nullptr;
	}

	return result;
}//CreateBrickFromColor()

XMVECTOR Brick::RenderColorFromHealth( std::int8_t health ) {
	if ( health == 0 ) {
		return Colors::Transparent;
	} else if ( health == 1 ) {
		return Colors::Green;
	} else if ( health == 2 ) {
		return Colors::Blue;
	} else if ( health == 3 ) {
		return Colors::Yellow;
	} else if ( health == 4 ) {
		return Colors::Red;
	} else {
		return Colors::White;
	}
}//RenderColorFromHealth()

bool Brick::IsRemoveReady( const std::shared_ptr<Brick> &brick ) {
	return !brick->IsVisible;
}//IsRemoveReady()
