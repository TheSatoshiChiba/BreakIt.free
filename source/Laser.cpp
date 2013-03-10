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
#include "Laser.h"
#include "Globals.h"

using namespace BreakIt;
using namespace BreakIt::Objects;
using namespace DirectX;

Laser::Laser( float x, float y, const std::shared_ptr<WinGame::Graphics::Texture2D> &texture ) :
	DrawableObject( x, y, 20, 32, texture ),
	Acceleration( 0.0f, 0.0f ),
	Velocity( 0.0f, -250.0f ) {

	HitBoxOffset = XMFLOAT2(
		2.0f,
		0.0f
		);
	HitBoxSize = XMFLOAT2( 16.0f, 32.0f );

	SourceRect.left		= 98;
	SourceRect.top		= 136;
	SourceRect.right	= SourceRect.left + Utility::ftoi( 20 );
	SourceRect.bottom	= SourceRect.top + Utility::ftoi( 32 );

	XMStoreFloat4( &RenderColor, Colors::White );
}//Ctor()

Laser::~Laser() {
}//Dtor()

void Laser::Update( float elapsedTime, float totalTime ) {
	auto acc = XMLoadFloat2( &Acceleration );
	auto vel = XMLoadFloat2( &Velocity );
	auto pos = XMLoadFloat2( &Position );

	auto oldVel = vel;
	vel = XMVectorAdd( vel, XMVectorScale( acc, elapsedTime ) );
	pos = XMVectorAdd( pos, XMVectorScale( XMVectorAdd( oldVel, vel ), elapsedTime ) );

	XMStoreFloat2( &Velocity, vel );
	XMStoreFloat2( &Position, pos );
}//Update()

bool Laser::IsRemoveReady( const std::unique_ptr<Laser> &laser ) {
	return !laser->IsVisible;
}//IsRemoveReady()
