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
#include "VirtualResolution.h"

using namespace DirectX;
using namespace WinGame;
using namespace WinGame::Graphics;

VirtualResolution::VirtualResolution() :
	Width( 1024.0f ),
	Height( 768.0f ),
	viewport( 0.0f, 0.0f, 1024.0f, 768.0f ) {
}//Ctor()

VirtualResolution::VirtualResolution( float width, float height ) :
	Width( width ),
	Height( height ),
	viewport( 0.0f, 0.0f, width, height ) {
}//Ctor()

bool VirtualResolution::IsWidescreen() const {
	return !( Width / Height <= 1.34 );
}//IsWidescreen()

float VirtualResolution::GetAspectRatio() const {
	return Width / Height;
}//GetAspectRatio()

XMMATRIX VirtualResolution::GetScaleMatrix( const GraphicsManager* graphics ) const {
	return XMMatrixScaling(
		viewport.Width / Width,
		viewport.Height / Height,
		1.0f
		);
}//GetScaleMatrix()

void VirtualResolution::SetVirtualViewport( GraphicsManager* graphics ) {
	//Calculate Viewport
	auto gWidth				= graphics->GetViewportWidth();
	auto gHeight			= graphics->GetViewportHeight();
	auto targetAspectRatio	= GetAspectRatio();
	auto width				= gWidth;
	auto height				= width / targetAspectRatio + 0.5f;

	if ( height > gHeight ) {
		height	= gHeight;
		width	= height * targetAspectRatio + 0.5f;
	}

	viewport.TopLeftX	= gWidth * 0.5f - width * 0.5f;
	viewport.TopLeftY	= gHeight * 0.5f - height * 0.5f;
	viewport.Width		= width;
	viewport.Height		= height;

	//Set Viewport
	graphics->SetViewport( viewport );
}//SetVirtualViewport()

XMVECTOR VirtualResolution::Unproject( const GraphicsManager* graphics, FXMVECTOR point ) const {
	// Compute the matrix.
	float xScale	= ( viewport.Width  > 0 ) ? 2.0f / viewport.Width : 0.0f;
    float yScale	= ( viewport.Height > 0 ) ? 2.0f / viewport.Height : 0.0f;
	auto identity	= XMMatrixIdentity();

	XMMATRIX mat = XMMATRIX(
		xScale, 0,			0, 0, 
		0,		-yScale,	0, 0, 
		0,		0,			1, 0, 
		-1,		1,			0, 1
		);

	return XMVector3Unproject(
		point,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		identity,
		identity,
		XMMatrixMultiply( GetScaleMatrix( graphics ), mat )
		);
}//Unproject()
