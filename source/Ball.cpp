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
#include "Ball.h"
#include "Globals.h"

using namespace BreakIt;
using namespace BreakIt::Objects;
using namespace DirectX;

Ball::Ball() :
	DrawableObject( 0.0f, 0.0f, BallTextureWidth, BallTextureHeight ),
	Acceleration( 0.0f, 0.0f ),
	Velocity( 0.0f, MinimumBallSpeed ),
	Power( 1 ),
	isBouncy( true ),
	faceTime( 0.0f ),
	isHit( false ),
	isInvisible( false ) {

	HitBoxOffset = XMFLOAT2(
		( BallTextureWidth - BallWidth ) * 0.5f,
		( BallTextureHeight - BallHeight ) * 0.5f
		);
	HitBoxSize = XMFLOAT2( BallWidth, BallHeight );

	SourceRect.left		= 0;
	SourceRect.top		= 34;
	SourceRect.right	= SourceRect.left + Utility::ftoi( BallTextureWidth );
	SourceRect.bottom	= SourceRect.top + Utility::ftoi( BallTextureHeight );

	XMStoreFloat4( &RenderColor, Colors::Orange );
}//Ctor()

Ball::Ball( float x, float y, const std::shared_ptr<WinGame::Graphics::Texture2D> &texture ) :
	DrawableObject( x, y, BallTextureWidth, BallTextureHeight, texture ),
	Acceleration( 0.0f, 0.0f ),
	Velocity( 0.0f, MinimumBallSpeed ),
	Power( 1 ),
	isBouncy( true ),
	faceTime( 0.0f ),
	isHit( false ),
	isInvisible( false ) {

	HitBoxOffset = XMFLOAT2(
		( BallTextureWidth - BallWidth ) * 0.5f,
		( BallTextureHeight - BallHeight ) * 0.5f
		);

	HitBoxSize = XMFLOAT2( BallWidth, BallHeight );

	SourceRect.left		= 0;
	SourceRect.top		= 34;
	SourceRect.right	= SourceRect.left + Utility::ftoi( BallTextureWidth );
	SourceRect.bottom	= SourceRect.top + Utility::ftoi( BallTextureHeight );

	XMStoreFloat4( &RenderColor, Colors::Orange );
}//Ctor()

Ball::~Ball() {
}//Dtor()

bool Ball::IsBouncy() const {
	return isBouncy;
}//IsBouncy()

bool Ball::IsInvisible() const {
	return isInvisible;
}//IsInvisible()

std::uint8_t Ball::GetPower() const {
	return Power;
}//GetPower()

void Ball::SetInvisibility( bool invisible ) {
	isInvisible = invisible;
	SetPower( Power );
}//SetInvisibility()

void Ball::SetPower( std::uint8_t power ) {
	Power = power;

	if ( Power >= MaximumBrickHealth ) {
		isBouncy			= false;
		SourceRect.left		= 32;
		SourceRect.right	= SourceRect.left + Utility::ftoi( BallTextureWidth );

		XMStoreFloat4( &RenderColor, Colors::White );
	} else if ( Power == 0 ) {
		isBouncy			= true;
		SourceRect.left		= 96;
		SourceRect.right	= SourceRect.left + Utility::ftoi( BallTextureWidth );

		XMStoreFloat4( &RenderColor, Colors::White );
	} else {
		isBouncy			= true;
		SourceRect.left		= 0;
		SourceRect.right	= SourceRect.left + Utility::ftoi( BallTextureWidth );

		XMStoreFloat4( &RenderColor, Colors::Orange );
	}

	if ( isInvisible ) {
		SourceRect.left		= 64;
		SourceRect.right	= SourceRect.left + Utility::ftoi( BallTextureWidth );

		XMStoreFloat4( &RenderColor, XMVectorSet( 0.1f, 0.1f, 0.1f, 0.2f ) );
	}
}//SetPower()

void Ball::Bounce( const Player *player, FXMVECTOR depth, FXMVECTOR normal ) {
	auto n			= XMVectorClamp( normal, XMVectorNegate( XMVectorSplatOne() ), XMVectorSplatOne() );
	auto correction = XMVectorMultiply( depth, n );
	auto pos		= XMLoadFloat2( &Position );
	auto vel		= XMLoadFloat2( &Velocity );
	auto paddleSize = XMLoadFloat2( &player->HitBoxSize );
	auto distance	= XMVectorSubtract( GetHitBoxCenter(), player->GetHitBoxCenter() );
	auto maxSpeed	= XMVectorSet( MinimumBallSpeed, MinimumBallSpeed, MinimumBallSpeed, MinimumBallSpeed );

	pos = XMVectorAdd( pos, correction );
	XMStoreFloat2( &Position, pos );

	distance = XMVectorDivide( distance, XMVectorScale( paddleSize, 0.5f ) );

	if ( XMVector2LessOrEqual( XMVector2Dot( vel, n ), XMVectorZero() ) ) {
		vel = XMVector2Reflect( vel, n );
	}

	vel = XMVectorMergeXY(
		XMVectorScale(
			XMVectorSplatX( distance ), 
			MinimumBallSpeed
			), 
		XMVectorSplatY( vel )
		);

	vel = XMVectorClamp( vel, XMVectorNegate( maxSpeed ), maxSpeed );
	XMStoreFloat2( &Velocity, vel );

	if ( isBouncy && XMVector2Greater( depth, XMVectorZero() ) ) {
		isHit		= true;
		faceTime	= 0.0f;

		if ( !isInvisible ) {
			XMStoreFloat4( &RenderColor, Colors::LightYellow );
		}
	}
}//Steer()

//void Ball::Bounce(FXMVECTOR depth, FXMVECTOR normal)
//{
//	auto n = XMVectorClamp(normal, XMVectorNegate(XMVectorSplatOne()), XMVectorSplatOne());
//	auto correction = XMVectorMultiply(depth, n);
//	auto pos = XMLoadFloat2(&Position);
//	auto vel = XMLoadFloat2(&Velocity);
//	auto maxSpeed = XMVectorSet(MinimumBallSpeed, MinimumBallSpeed, MinimumBallSpeed, MinimumBallSpeed);
//
//	pos = XMVectorAdd(pos, correction);
//	XMStoreFloat2(&Position, pos);
//	
//	if(XMVector2LessOrEqual(XMVector2Dot(vel, n), XMVectorZero()))
//	{
//		vel = XMVector2Reflect(vel, n);
//	}
//
//	vel = XMVectorClamp(vel, XMVectorNegate(maxSpeed), maxSpeed);
//	XMStoreFloat2(&Velocity, vel);
//}//BounceBall()

void Ball::Bounce( FXMVECTOR correction, FXMVECTOR maxDepth, FXMVECTOR maxNormal ) {
	auto maxSpeed	= XMVectorSet( MinimumBallSpeed, MinimumBallSpeed, MinimumBallSpeed, MinimumBallSpeed );
	auto pos		= XMLoadFloat2( &Position );
	pos				= XMVectorAdd( pos, correction );

	XMStoreFloat2( &Position, pos );

	//TODO: We still need to find out how to reflect the Ball according to the Normal.
	//		There are still some edge cases present where the ball behaves odd. for example
	//		if the maxNormal is in the 90 degree way to the velocity. ---\ 
	//		That makes the ball slide around the edge of a block and it won't be reflected
	//		In any direction. That needs to be fixed in here.
	//		First try would be to take the Velocity Normal into account and make some Dot product
	//		Calculations to see if it complies according to the maxNormal.
	//		(Let's hope this will be easy to fix *haha*)
	auto vel = XMLoadFloat2( &Velocity );

	if ( XMVector2GreaterOrEqual( XMVector2Dot( vel, maxNormal ), XMVectorZero() ) ) {
	} else {
		vel = XMVector2Reflect( vel, maxNormal ); 		
	}

	vel = XMVectorClamp( vel, XMVectorNegate( maxSpeed ), maxSpeed );
	XMStoreFloat2( &Velocity, vel );

	if ( isBouncy && XMVector2Greater( maxDepth, XMVectorZero() ) ) {
		isHit		= true;
		faceTime	= 0.0f;

		if ( !isInvisible ) {
			XMStoreFloat4( &RenderColor, Colors::LightYellow );
		}
	}
}//Bounce()

void Ball::Update( float elapsedTime, float totalTime ) {
	auto acc	= XMLoadFloat2( &Acceleration );
	auto vel	= XMLoadFloat2( &Velocity );
	auto pos	= XMLoadFloat2( &Position );
	auto oldVel = vel;

	vel = XMVectorAdd( vel, XMVectorScale( acc, elapsedTime ) );
	pos = XMVectorAdd( pos, XMVectorScale( XMVectorAdd( oldVel, vel ), elapsedTime ) );

	XMStoreFloat2( &Velocity, vel );
	XMStoreFloat2( &Position, pos );

	if ( isHit ) {
		faceTime += elapsedTime;

		if ( faceTime >= 0.1f ) {
			faceTime	= 0.0f;
			isHit		= false;

			SetPower( Power );
		}
	}
}//Update()

bool Ball::IsRemoveReady( const std::unique_ptr<Ball> &ball ) {
	return !ball->IsVisible;
}//IsRemoveReady()
