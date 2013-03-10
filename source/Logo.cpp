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
#include "Logo.h"

using namespace BreakIt;
using namespace BreakIt::GUI;
using namespace DirectX;

Logo::Logo( float x, float y, float width, float height, const std::shared_ptr<WinGame::Graphics::Texture2D> &texture ) :
	GUIObject( x, y, width, height, texture ),
	AngleDifference( 0.0f ),
	ScaleDifference( 0.0f ),
	ScaleFactor( 1.0f ),
	RenderScale( 1.0f ) {

	SourceRect.left		= 0;
	SourceRect.top		= 0;
	SourceRect.right	= SourceRect.left + 512;
	SourceRect.bottom	= SourceRect.top + 256;
}//Ctor()

Logo::~Logo() {
}//Dtor()

void Logo::Update( float elapsedTime, float totalTime ) {
	//Scale Logo
	float maxScale = 1.0f + std::fabsf( ScaleDifference );
	float minScale = 1.0f - std::fabsf( ScaleDifference );

	RenderScale += ScaleFactor * elapsedTime;
	if ( !Utility::InRange<float>( RenderScale, minScale, maxScale ) ) {
		RenderScale = std::max<float>( std::min<float>( RenderScale, maxScale ), minScale );
		ScaleFactor = -ScaleFactor;
	}

	//Rotate Logo
	float maxAngle = std::fabsf( AngleDifference ) - 0.2f;
	float minAngle = -maxAngle + 0.2f;

	Rotation += AngleDifference * elapsedTime;
	if ( !Utility::InRange<float>( Rotation, minAngle, maxAngle ) ) {
		Rotation		= std::max<float>( std::min<float>( Rotation, maxAngle ), minAngle );
		AngleDifference = -AngleDifference;
	}
}//Update()

void Logo::Draw( SpriteBatch *batch ) {
	if ( !Texture || !Texture->IsInitialized() || !IsVisible ) {
		return;
	}

	RECT dest = { 
		static_cast<std::int32_t>( Position.x ), 
		static_cast<std::int32_t>( Position.y ), 
		static_cast<std::int32_t>( Position.x ) + static_cast<std::int32_t>( Size.x * RenderScale ), 
		static_cast<std::int32_t>( Position.y ) + static_cast<std::int32_t>( Size.y * RenderScale )
	};

	batch->Draw(
		Texture->GetResourceView(),
		dest,
		&SourceRect,
		XMLoadFloat4( &RenderColor ),
		XMConvertToRadians( Rotation ),
		Origin
		);
}//Draw()
