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
#include "DrawableObject.h"

using namespace WinGame;
using namespace WinGame::Graphics;
using namespace DirectX;
using namespace BreakIt;
using namespace BreakIt::Objects;

DrawableObject::DrawableObject() :
	GameObject(),
	IDrawableObject() {
}//Ctor()

DrawableObject::DrawableObject( float x, float y, float width, float height ) :
	GameObject( x, y, width, height ),
	IDrawableObject() {

	SourceRect.left		= 0;
	SourceRect.top		= 0;
	SourceRect.right	= Utility::ftoi( width );
	SourceRect.bottom	= Utility::ftoi( height );
}//Ctor()

DrawableObject::DrawableObject( float x, float y, float width, float height, const std::shared_ptr<Texture2D> &texture ) :
	GameObject( x, y, width, height ),
	IDrawableObject() {

	SourceRect.left		= 0;
	SourceRect.top		= 0;
	SourceRect.right	= texture && texture->IsInitialized() ? texture->GetWidth() : Utility::ftoi( width );
	SourceRect.bottom	= texture && texture->IsInitialized() ? texture->GetHeight() : Utility::ftoi( height );

	Texture = texture;
}//Ctor()

DrawableObject::~DrawableObject() {
}//Dtor()

void DrawableObject::Draw( SpriteBatch *batch ) {
	if ( !Texture || !Texture->IsInitialized() || !IsVisible ) {
		return;
	}

	RECT dest = {
		Utility::ftoi( Position.x ),
		Utility::ftoi( Position.y ),
		Utility::ftoi( Position.x + Size.x ),
		Utility::ftoi( Position.y + Size.y )
	};

	batch->Draw( Texture->GetResourceView(), dest, &SourceRect, XMLoadFloat4( &RenderColor ), XMConvertToRadians( Rotation ), Origin );

	//Draw Hitbox
	//RECT dest2 = 
	//{
	//	Utility::ftoi(Position.x + HitBoxOffset.x),
	//	Utility::ftoi(Position.y + HitBoxOffset.y),
	//	Utility::ftoi(Position.x + HitBoxOffset.x + HitBoxSize.x),
	//	Utility::ftoi(Position.y + HitBoxOffset.y + HitBoxSize.y)
	//};

	//RECT source =
	//{
	//	Utility::ftoi(62),
	//	Utility::ftoi(19),
	//	Utility::ftoi(63),
	//	Utility::ftoi(20)
	//};

	//batch->Draw(Texture->GetResourceView(), dest2, &source, XMLoadFloat4(&RenderColor), XMConvertToRadians(Rotation), Origin);
}//Draw()
