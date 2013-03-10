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

#include "InputTypes.h"

namespace WinGame {
	namespace Input {

		class InputManager {
		public:
			explicit InputManager();
			virtual	~InputManager();
			UTILITY_CLASS_MOVE( InputManager );

			DirectX::XMVECTOR	GetFingerPosition( FingerIndex finger = FingerIndex::FirstFinger ) const;
			DirectX::XMVECTOR	GetFingerPositionDelta( FingerIndex finger = FingerIndex::FirstFinger ) const;
			DirectX::XMVECTOR	GetMousePosition() const;
			DirectX::XMVECTOR	GetMousePositionDelta() const;
			int					GetMouseWheelDelta() const;

			bool IsFingerInContact( FingerIndex finger = FingerIndex::FirstFinger ) const;
			bool IsLeftMouseButtonPressed() const;
			bool IsRightMouseButtonPressed() const;
			bool IsMiddleMouseButtonPressed() const;

			bool IsFingerInContactOnce( FingerIndex finger = FingerIndex::FirstFinger ) const;
			bool IsFingerNotInContactOnce( FingerIndex finger = FingerIndex::FirstFinger ) const;
			bool IsLeftMouseButtonPressedOnce() const;
			bool IsRightMouseButtonPressedOnce() const;
			bool IsMiddleMouseButtonPressedOnce() const;
			bool IsLeftMouseButtonReleasedOnce() const;
			bool IsRightMouseButtonReleasedOnce() const;
			bool IsMiddleMouseButtonReleasedOnce() const;

			void Initialize();
			void Update();
			void ShowMousePointer();
			void HideMousePointer();

			void SetPointerAction( InputAction action, Windows::UI::Core::PointerEventArgs ^args );
			void SetMouseDelta( Windows::Devices::Input::MouseEventArgs ^args );

		private:
			UTILITY_CLASS_COPY( InputManager );
			UTILITY_CLASS_PIMPL();

		};//InputManager class

	}//Input namespace
}//WinGame namespace
