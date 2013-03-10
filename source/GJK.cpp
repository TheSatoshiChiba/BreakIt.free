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
#include "GJK.h"

using namespace DirectX;

bool Utility::GJKSimplexCheck( std::vector<XMFLOAT3>& simplex, XMVECTOR& direction ) {
	XMVECTOR S0, S1, SN;

	auto a	= simplex.back();
	SN		= XMLoadFloat3( &a );
	S0		= XMLoadFloat3( &simplex[0] );
	S1		= XMLoadFloat3( &simplex[1] );

	auto ao = XMVectorNegate( SN );

	if ( simplex.size() == 3 ) {
		auto ab = XMVectorSubtract( S1, SN );
		auto ac = XMVectorSubtract( S0, SN );

		auto abPrep = Utility::TripleProduct( ac, ab, ab );
		auto acPrep = Utility::TripleProduct( ab, ac, ac );

		if ( XMVector2GreaterOrEqual(
			XMVector2Dot( acPrep, ao ), 
			g_XMZero
			) ) {
			simplex.erase( std::begin( simplex ) + 1 );
			direction = acPrep;
		} else {
			if ( XMVector2Less(
				XMVector2Dot( abPrep, ao ), 
				g_XMZero
				) ) {
				return true;
			} else {
				simplex.erase( std::begin( simplex ) );
				direction = abPrep;
			}
		}
	} else {
		auto ab = XMVectorSubtract( S0, SN );

		direction = Utility::TripleProduct( ab, ao, ab );
		if ( XMVector2NearEqual(
			direction, 
			g_XMZero, 
			g_XMEpsilon
			) ) {
			direction = XMVectorNegate( XMVector2Orthogonal( ab ) );
		}
	}

	return false;
}//GJKSimplexCheck()

void Utility::EPAFindClosestEdge( std::vector<XMFLOAT3>& simplex, Utility::SIMPLEX_WINDING winding, XMVECTOR& depth, XMVECTOR& normal, unsigned int& index )
{
	auto size = simplex.size();
	depth = g_FltMax;

	XMVECTOR n, d;
	XMVECTOR SY, SX;

	for ( decltype( size ) x = 0; x < size; ++x ) {
		decltype( size ) y = x + 1 == size ? 0 : x + 1;

		SY	= XMLoadFloat3( &simplex[y] );
		SX	= XMLoadFloat3( &simplex[x] );
		n	= XMVectorSubtract( SY, SX );

		if ( winding == Utility::SIMPLEX_WINDING::CLOCKWISE ) {
			n = XMVector2Orthogonal( n );
		} else {
			n = XMVectorNegate( XMVector2Orthogonal( n ) );
		}

		n = XMVector2Normalize( n );
		d = XMVectorAbs( XMVector2Dot( SX, n ) );

		if ( XMVector2Less( d, depth ) ) {
			depth	= d;
			normal	= n;
			index	= static_cast<unsigned int>( y );
		}
	}
}//EPAFindClosestEdge()
