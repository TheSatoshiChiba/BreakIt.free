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
#include "Player.h"
#include "Globals.h"
#include "ItemManager.h"
#include "BrickManager.h"

using namespace BreakIt;
using namespace BreakIt::Objects;
using namespace DirectX;

Player::Player( float x, float y, const std::shared_ptr<WinGame::Graphics::Texture2D> &texture ) :
	DrawableObject( x, y, PlayerTextureWidth, PlayerTextureHeight, texture ),
	Health( PlayerStartHealth ),
	Points( 0 ),
	growSize( 2 ),
	leftOffset( 130 ),
	deltaX( 0.0f ),
	laserActivated( false ),
	laserTime( false ),
	laserCount( 0 ),
	laserShots( 0 ),
	TempPoints( 0 ),
	TempHealth( 0 ) {

	HitBoxOffset = XMFLOAT2( 0.0f, 0.0f );
	ResetGrowth();

	SourceRect.left		= leftOffset;
	SourceRect.top		= 60;
	SourceRect.right	= SourceRect.left + Utility::ftoi( PlayerTextureWidth );
	SourceRect.bottom	= SourceRect.top + Utility::ftoi( PlayerTextureHeight );

	canon = std::make_unique<DrawableObject>(
		0.0f,
		0.0f,
		32.0f,
		16.0f,
		texture
		);

	canon->SourceRect.left		= 0;
	canon->SourceRect.top		= 222;
	canon->SourceRect.right		= canon->SourceRect.left + Utility::ftoi( 32 );
	canon->SourceRect.bottom	= canon->SourceRect.top + Utility::ftoi( 16 );

	XMStoreFloat4( &RenderColor, Colors::White );
}//Ctor()

Player::~Player() {
}//Dtor()

bool Player::IsLaserActive() const {
	return laserActivated;
}//IsLaserActive()

int Player::GetLaserCount() const {
	return 5 - laserCount;
}//GetLaserCount()

void Player::ResetLaser() {
	laserActivated	= false;
	laserTime		= 0.0f;
	laserCount		= 0;

	laserShots.clear();
}//ResetLaser()

void Player::ActivateLaser() {
	laserActivated	= true;
	laserTime		= 0.0f;
	laserCount		= 0;
}//ActivateLaser()

void Player::Update( float elapsedTime, float totalTime, SoundManager *sounds, BrickManager *bricks, ItemManager *items ) {
	if ( laserActivated ) {
		laserTime += elapsedTime;

		if ( laserTime >= 1.0f ) {
			++laserCount;
			laserTime -= 1.0f;

			//auto middle = growSize / 2;
			float posX = Position.x;

			for ( decltype( growSize ) i = 0; i <= growSize; ++i ) {
				sounds->PlaySound( SOUND_FILE::LASER_SHOT );
				laserShots.push_back(
				std::move(
					std::make_unique<Laser>(
						posX + PlayerWidth * 0.5f - 16.0f,
						Position.y - 10.0f,
						Texture
						)
					)
				);

				posX += Size.x - 2.0f;
			}
		}

		if ( laserCount >= 5 ) {
			laserActivated	= false;
			laserTime		= 0.0f;
			laserCount		= 0;
		}
	}

	if ( laserShots.size() != 0 ) {
		for ( auto &shot : laserShots ) {
			if ( shot->IsVisible ) {
				shot->Update( elapsedTime, totalTime );

				if ( shot->Position.y < -32.0f ) {
					shot->IsVisible = false;
				}

				if ( shot->IsVisible ) {
					bricks->CheckCollision( shot.get(), this, sounds, items );
				}
			}
		}

		laserShots.erase(
			std::remove_if(
				std::begin( laserShots ),
				std::end( laserShots ),
				Laser::IsRemoveReady
				),
			std::end( laserShots )
			);
	}
}//Update()

void Player::Move( FXMVECTOR delta ) {
	if ( XMVector2NotEqual( delta, XMVectorZero() ) ) {
		deltaX += XMVectorGetX( XMVectorAbs( delta ) );

		if ( deltaX > 10.0f ) {
			deltaX = 0.0f;
			leftOffset += Utility::ftoi( PlayerTextureWidth );

			if ( leftOffset > SourceRect.left + PlayerTextureWidth ) {
				leftOffset = SourceRect.left;
			}
		}
	}

	auto pos	= XMLoadFloat2( &Position );
	pos			= XMVectorAdd( pos, delta );

	XMStoreFloat2( &Position, pos );
}//Move()

void Player::Clamp( FXMVECTOR leftBorderMax, FXMVECTOR rightBorderMin ) {
	auto offset = XMLoadFloat2( &HitBoxOffset );
	auto pos	= XMLoadFloat2( &Position );
	auto size	= XMLoadFloat2( &HitBoxSize );

	pos = XMVectorAdd( pos, offset );
	//auto leftBorder = XMVectorSubtract(
	//	leftBorderMax,
	//	offset
	//	);

	//auto rightBorder = XMVectorAdd(
	//	rightBorderMin,
	//	offset
	//	);

	pos = XMVectorClamp(
		pos, 
		leftBorderMax,
		XMVectorSubtract(
			rightBorderMin,
			size
			)
		);

	pos = XMVectorSubtract( pos, offset );

	XMStoreFloat2( &Position, pos );
}//Clamp()

void Player::Grow() {
	if ( growSize <= 4 ) {
		growSize		+=2;
		HitBoxSize.x	+= ( PlayerWidth * 2.0f ) - 4.0f;
		Position.x		-= PlayerWidth + 2.0f;
	}
}//Grow()

void Player::Shrink() {
	if ( growSize != 0 ) {
		HitBoxSize.x	-= ( PlayerWidth * 2.0f ) - 4.0f;
		growSize		-=2;
		Position.x		+= PlayerWidth - 2.0f;
	}
}//Shrink()

void Player::ResetGrowth() {
	growSize		= 2;
	float offset	= 2.0f * growSize;
	HitBoxSize		= XMFLOAT2( PlayerWidth * static_cast<float>( growSize + 1 ) - offset , PlayerHeight );
}//ResetGrowth()

void Player::Draw( DirectX::SpriteBatch *batch ) {
	if ( !Texture || !Texture->IsInitialized() || !IsVisible ) {
		return;
	}

	RECT src;
	RECT dest;

	//Set left tile
	LONG topVal = 60;
	src.left	= leftOffset;
	src.top		= topVal;
	src.right	= src.left + Utility::ftoi( PlayerTextureWidth );
	src.bottom	= src.top + Utility::ftoi( PlayerTextureHeight );

	auto	middle	= growSize / 2;
	float	posX	= Position.x;

	for ( decltype( growSize ) i = 0; i <= growSize; ++i ) {
		if ( i == middle ) {
			src.left	= leftOffset;
			src.top		= topVal + Utility::ftoi( PlayerTextureHeight );
			src.right	= src.left + Utility::ftoi( PlayerTextureWidth );
			src.bottom	= src.top + Utility::ftoi( PlayerTextureHeight );
		} else if ( i > middle ) {
			src.left	= leftOffset;
			src.top		= topVal + Utility::ftoi( PlayerTextureHeight ) * 2;
			src.right	= src.left + Utility::ftoi( PlayerTextureWidth );
			src.bottom	= src.top + Utility::ftoi( PlayerTextureHeight );
		}

		dest.left	= Utility::ftoi( posX );
		dest.top	= Utility::ftoi( Position.y );
		dest.right	= dest.left + Utility::ftoi( Size.x );
		dest.bottom = dest.top + Utility::ftoi( Size.y );
		
		canon->Position.x = posX;
		canon->Position.y = Position.y;

		posX += Size.x - 2.0f;
		
		batch->Draw( Texture->GetResourceView(), dest, &src, XMLoadFloat4( &RenderColor ), XMConvertToRadians( Rotation ), Origin );
		
		if ( laserActivated ) {
			canon->Draw( batch );
		}
	}

	//DRAW LASER
	if ( laserShots.size() != 0 ) {
		for ( auto &shot : laserShots ) {
			shot->Draw( batch );
		}
	}
}//Draw()
