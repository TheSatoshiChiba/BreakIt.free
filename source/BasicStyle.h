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

#include "IStyle.h"

namespace BreakIt {
	namespace Styles {
		class BasicStyle : public IStyle {
		public:
			explicit BasicStyle();
			virtual ~BasicStyle();
			UTILITY_CLASS_MOVE( BasicStyle );

			virtual const wchar_t*	GetStyleTexture() const override;
			virtual std::uint8_t	GetLevelCount() const override;
			virtual const wchar_t*	GetLevelFilename( std::uint8_t index ) const override;
			virtual const wchar_t*	GetBackgroundMusic() const override;

			virtual const wchar_t* GetSideHitSound() const override;
			virtual const wchar_t* GetBallLostSound() const override;
			virtual const wchar_t* GetPlayerHitSound() const override;
			virtual const wchar_t* GetBrickHitSound() const override;
			virtual const wchar_t* GetBrickBreakSound() const override;
			virtual const wchar_t* GetGameWonSound() const override;
			virtual const wchar_t* GetGameLostSound() const override;
			virtual const wchar_t* GetPointsAddSound() const override;
			virtual const wchar_t* GetHealthAddSound() const override;
			virtual const wchar_t* GetPositiveItemSound() const override;
			virtual const wchar_t* GetNegativeItemSound() const override;
			virtual const wchar_t* GetLaserShotSound() const override;

		private:
			UTILITY_CLASS_COPY( BasicStyle );
			UTILITY_CLASS_PIMPL();

		};//BasicStyle class

	}//Styles namespace
}//BreakIt namespace
