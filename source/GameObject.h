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

#include "IGameObject.h"

namespace BreakIt {
	namespace Objects {
		class GameObject : public IGameObject {
		public:
			explicit GameObject();
			GameObject( float x, float y, float width, float height );
			virtual ~GameObject();

			virtual void Update( float elapsedTime, float totalTime ) override;

			bool IsTouching( const GameObject *other ) const;
			bool IsTouching( const GameObject *other, DirectX::XMVECTOR &penetrationDepth, DirectX::XMVECTOR &penetrationNormal ) const;
			DirectX::XMVECTOR GetFarthestPoint( DirectX::FXMVECTOR direction ) const;

			DirectX::XMVECTOR GetHitBoxCenter() const;
			DirectX::XMVECTOR GetHitBoxTopLeft() const;
			DirectX::XMVECTOR GetHitBoxTopRight() const;
			DirectX::XMVECTOR GetHitBoxBottomLeft() const;
			DirectX::XMVECTOR GetHitBoxBottomRight() const;

			DirectX::XMFLOAT2 HitBoxOffset;
			DirectX::XMFLOAT2 HitBoxSize;

		private:
			DirectX::XMVECTOR MinkowskiSupport(
				const GameObject *other, 
				DirectX::FXMVECTOR direction
				) const;

			void EPAPenetration(
				std::vector<DirectX::XMFLOAT3> &simplex, 
				const GameObject *other,
				DirectX::XMVECTOR &penetrationDepth,
				DirectX::XMVECTOR &penetrationNormal
				) const;

		};//GameObject class

	}//Objects namespace
}//BreakIt namespace
