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
#include "GameObject.h"

using namespace BreakIt;
using namespace BreakIt::Objects;
using namespace DirectX;

GameObject::GameObject() :
	IGameObject(),
	HitBoxOffset( 0.0f, 0.0f ) {
	HitBoxSize = XMFLOAT2( Size.x, Size.y );
}//Ctor()

GameObject::GameObject( float x, float y, float width, float height ) :
	IGameObject(),
	HitBoxOffset( 0.0f, 0.0f ),
	HitBoxSize( width, height ) {

	Position.x	= x;
	Position.y	= y;
	Size.x		= width;
	Size.y		= height;
}//Ctor()

GameObject::~GameObject() {
}//Dtor()

void GameObject::Update( float elapsedTime, float totalTime ) {
}//Update()

XMVECTOR GameObject::GetHitBoxCenter() const {
	auto pos	= XMVectorAdd( XMLoadFloat2( &Position ), XMLoadFloat2( &HitBoxOffset ) );
	auto size	= XMLoadFloat2( &HitBoxSize );
				
	return XMVectorAdd(
		pos, 
		XMVectorScale( size, 0.5f )
		);
}//GetCenter()

XMVECTOR GameObject::GetHitBoxTopLeft() const {
	return XMVectorAdd( XMLoadFloat2( &Position ), XMLoadFloat2( &HitBoxOffset ) );
}//GetTopLeft()

XMVECTOR GameObject::GetHitBoxTopRight() const {
	XMFLOAT2 p(
		Position.x + HitBoxOffset.x + HitBoxSize.x, 
		Position.y + HitBoxOffset.y
		);

	return XMLoadFloat2( &p );
}//GetTopRight()

XMVECTOR GameObject::GetHitBoxBottomLeft() const {
	XMFLOAT2 p(
		Position.x + HitBoxOffset.x, 
		Position.y + HitBoxOffset.y + HitBoxSize.y
		);

	return XMLoadFloat2( &p );
}//GetBottomLeft()

XMVECTOR GameObject::GetHitBoxBottomRight() const {
	auto p = XMVectorAdd( XMLoadFloat2( &Position ), XMLoadFloat2( &HitBoxOffset ) );
	auto s = XMLoadFloat2( &HitBoxSize );

	return XMVectorAdd( p, s );
}//GetBottomRight()

XMVECTOR GameObject::GetFarthestPoint( DirectX::FXMVECTOR direction ) const {
	auto vecs = std::vector<XMFLOAT3>( 3 );

	XMStoreFloat3( &vecs[0], GetHitBoxTopRight() );
	XMStoreFloat3( &vecs[1], GetHitBoxBottomLeft() );
	XMStoreFloat3( &vecs[2], GetHitBoxBottomRight() );

	/*vecs[0] =;
	vecs[1] = GetHitBoxBottomLeft();
	vecs[2] = GetHitBoxBottomRight();*/
	
	auto ret	= GetHitBoxTopLeft();
	auto proj	= XMVector2Dot( direction, ret );
	auto max	= proj;

	XMVECTOR tmp;
	for ( auto &vec : vecs ) {
		tmp		= XMLoadFloat3( &vec );
		proj	= XMVector2Dot( direction, tmp );

		if ( XMVector2Greater( proj, max ) ) {
			ret = tmp;
			max = proj;
		}
	}

	return ret;
}//GetFarthestPoint()

DirectX::XMVECTOR GameObject::MinkowskiSupport( const GameObject *other, DirectX::FXMVECTOR direction ) const {
	auto a = GetFarthestPoint( direction );
	auto b = other->GetFarthestPoint( DirectX::XMVectorNegate( direction ) );

	return DirectX::XMVectorSubtract( a, b );
}//MinkowskiSupport()

void GameObject::EPAPenetration(
	std::vector<XMFLOAT3> &simplex,
	const GameObject *other,
	XMVECTOR &penetrationDepth,
	XMVECTOR &penetrationNormal ) const {

	auto winding = Utility::GetSimplexWinding( simplex );
	auto epsilon =  XMVectorSqrt( XMVectorSplatEpsilon() );

	XMFLOAT3		tp;
	XMVECTOR		n;
	XMVECTOR		d;
	XMVECTOR		p;
	XMVECTOR		proj;
	std::uint32_t	index;

	n = XMVectorZero();
	p = XMVectorZero();

	for ( std::int16_t i = 0; i < 100; ++i ) {
		Utility::EPAFindClosestEdge( simplex, winding, d, n, index );

		p		= MinkowskiSupport( other, n );
		proj	= XMVector2Dot( p, n );

		if ( XMVector2Less( XMVectorSubtract( proj, d ), epsilon ) ) {
			penetrationNormal	= n;
			penetrationDepth	= proj;
			return;
		}

		XMStoreFloat3( &tp, p );
		simplex.insert( std::begin( simplex ) + index, tp );
	}

#if _DEBUG
	UTILITY_DEBUG_MSG( L"Warning: EPAPenetration() hit the maximum interation number!\n" );
#endif

	penetrationNormal	= n;
	penetrationDepth	= XMVector2Dot( p, n );
}//EPAPenetration()

bool GameObject::IsTouching( const GameObject *other, XMVECTOR &penetrationDepth, XMVECTOR &penetrationNormal ) const {
	auto simplex	= std::vector<XMFLOAT3>();
	auto direction	= XMVectorSubtract( other->GetHitBoxCenter(), GetHitBoxCenter() );

	if ( XMVector2Equal( direction, g_XMZero ) ) {
		direction = XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f );
	}

	XMFLOAT3 simplexT;
	auto simplexV = MinkowskiSupport( other, direction );

	XMStoreFloat3( &simplexT, simplexV );
	simplex.push_back( simplexT );

	if ( XMVector2LessOrEqual( XMVector2Dot( simplexV, direction ), g_XMZero ) ) {
		return false;
	}

	direction = XMVectorNegate( direction );
	while ( true ) {
		simplexV = MinkowskiSupport( other, direction );
		XMStoreFloat3( &simplexT, simplexV );
		simplex.push_back( simplexT );

		if ( XMVector2LessOrEqual ( XMVector2Dot( simplexV, direction ), g_XMZero ) ) {
			break;
		} else {
			if ( Utility::GJKSimplexCheck( simplex, direction ) ) {
				EPAPenetration( simplex, other, penetrationDepth, penetrationNormal );
				return true;
			}
		}
	}

	return false;
}//IsTouching()

bool GameObject::IsTouching( const GameObject *other ) const {
	auto simplex	= std::vector<XMFLOAT3>();
	auto direction	= XMVectorSubtract( other->GetHitBoxCenter(), GetHitBoxCenter() );

	if ( XMVector2Equal( direction, g_XMZero ) ) {
		direction = XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f );
	}

	XMFLOAT3 simplexT;
	XMVECTOR simplexLast = MinkowskiSupport( other, direction );

	XMStoreFloat3( &simplexT, simplexLast );
	simplex.push_back( simplexT );

	if ( XMVector2LessOrEqual( XMVector2Dot( simplexLast, direction ), g_XMZero ) ) {
		return false;
	}

	direction = XMVectorNegate( direction );
	while ( true ) {
		simplexLast = MinkowskiSupport( other, direction );

		XMStoreFloat3( &simplexT, simplexLast );
		simplex.push_back( simplexT );

		if ( XMVector2LessOrEqual( XMVector2Dot( simplexLast, direction ), g_XMZero ) ) {
			break;
		} else {
			if ( Utility::GJKSimplexCheck( simplex, direction ) ) {
				return true;
			}
		}
	}

	return false;
}//IsTouching()
