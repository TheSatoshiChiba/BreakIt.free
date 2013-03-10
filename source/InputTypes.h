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

namespace WinGame {
	namespace Input {

		const std::uint8_t MaximumFingers = 5;

		enum FingerIndex : std::uint8_t {
			FirstFinger = 0x00,
			SecondFinger,
			ThirdFinger,
			FourthFinger,
			FifthFinger
		};//FingerIndex enum class

		enum class InputAction : std::uint8_t {
			Released = 0x00,
			Pressed,
			Moved,
			WheelChanged
		};//InputAction enum class

		struct FingerState {
			std::uint8_t	ID;
			bool			isInContact;

			DirectX::XMFLOAT2 Position;
			DirectX::XMFLOAT2 PositionDelta;

		};//FingerState struct

		struct MouseState {
			std::uint8_t		ID;
			DirectX::XMFLOAT2	Position;
			DirectX::XMFLOAT2	PositionDelta;

			bool			isLeftButtonPressed;
			bool			isRightButtonPressed;
			bool			isMiddleButtonPressed;
			std::int32_t	MouseWheelDelta;
		};//MouseState struct

	}//Input namespace
}//WinGame namespace
