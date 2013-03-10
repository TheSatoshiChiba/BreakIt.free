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

#pragma once

namespace Utility
{
	enum class SIMPLEX_WINDING : unsigned char {
		CLOCKWISE,
		COUNTER_CLOCKWISE
	};//SIMPLEX_WINDING enumeration

	inline DirectX::XMVECTOR TripleProduct( DirectX::FXMVECTOR v1, DirectX::FXMVECTOR v2, DirectX::FXMVECTOR v3 ) {
		auto a = DirectX::XMVectorMultiply(
			v2, 
			DirectX::XMVector2Dot( v1, v3 )
			);

		auto b = DirectX::XMVectorMultiply(
			v1, 
			DirectX::XMVector2Dot( v2, v3 )
			);

		return DirectX::XMVectorSubtract( a, b );
	}//TripleProduct()
 
	inline SIMPLEX_WINDING GetSimplexWinding( std::vector<DirectX::XMFLOAT3>& simplex ) {
		auto size = simplex.size();

		DirectX::XMVECTOR abCross;
		DirectX::XMVECTOR SX, SN;

		for ( decltype( size ) x = 0; x < size; ++x ) {
			SX = XMLoadFloat3( &simplex[x] );
			SN = XMLoadFloat3( &simplex[x + 1 == size ? 0 : x + 1] );

			abCross = DirectX::XMVector2Cross(
				SX, 
				SN
				);

			if ( DirectX::XMVector2Greater( abCross, DirectX::g_XMZero ) ) {
				return SIMPLEX_WINDING::COUNTER_CLOCKWISE;
			} else if( DirectX::XMVector2Less( abCross, DirectX::g_XMZero ) ) {
				return SIMPLEX_WINDING::CLOCKWISE;
			}
		}

		return SIMPLEX_WINDING::COUNTER_CLOCKWISE; //We just assume it's counter clockwise
	}//GetSimplexWinding()

	void EPAFindClosestEdge(
		std::vector<DirectX::XMFLOAT3>& simplex, 
		SIMPLEX_WINDING winding, 
		DirectX::XMVECTOR& depth, 
		DirectX::XMVECTOR& normal, 
		unsigned int& index );

	bool GJKSimplexCheck( std::vector<DirectX::XMFLOAT3>& simplex, DirectX::XMVECTOR& direction );
}//Utility namespace
