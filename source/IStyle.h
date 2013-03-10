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

namespace BreakIt {
	namespace Styles {
		class IStyle {
		public:
			virtual ~IStyle() {
			}

			//Texture Methods
			virtual const wchar_t* GetStyleTexture() const = 0;

			//Level Methods
			virtual std::uint8_t	GetLevelCount() const = 0;
			virtual const wchar_t*	GetLevelFilename( std::uint8_t index ) const = 0;

			//Sound Methods
			virtual const wchar_t* GetSideHitSound() const = 0;
			virtual const wchar_t* GetBallLostSound() const = 0;
			virtual const wchar_t* GetPlayerHitSound() const = 0;
			virtual const wchar_t* GetBrickHitSound() const = 0;
			virtual const wchar_t* GetBrickBreakSound() const = 0;
			virtual const wchar_t* GetGameWonSound() const = 0;
			virtual const wchar_t* GetGameLostSound() const = 0;
			virtual const wchar_t* GetPointsAddSound() const = 0;
			virtual const wchar_t* GetHealthAddSound() const = 0;
			virtual const wchar_t* GetPositiveItemSound() const = 0;
			virtual const wchar_t* GetNegativeItemSound() const = 0;
			virtual const wchar_t* GetLaserShotSound() const = 0;

			//Music Methods
			virtual const wchar_t* GetBackgroundMusic() const = 0;

		};//IStyle interface
	}//Styles namespace
}//BreakIt namespace
