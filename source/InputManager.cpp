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
#include "InputManager.h"

using namespace WinGame;
using namespace WinGame::Input;
using namespace DirectX;
using namespace Windows::Devices::Input;
using namespace Windows::UI::Core;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Input;

class InputManager::Impl {
public:
	Impl();

	FingerState	fingerState[MaximumFingers];
	FingerState oldFingerState[MaximumFingers];
	MouseState	mouseState;
	MouseState	oldMouseState;

	void InitMouse();
	void InitFingers();
	void SetFingerState( InputAction action, PointerPoint ^pointer );
	void SetMouseState( PointerPoint ^pointer );
	void SetMouseDelta( MouseEventArgs ^args );

};//Impl class

InputManager::Impl::Impl() {
}//Ctor()

void InputManager::Impl::InitMouse() {
	mouseState.ID						= 0U;
	mouseState.isLeftButtonPressed		= false;
	mouseState.isMiddleButtonPressed	= false;
	mouseState.isRightButtonPressed		= false;
	mouseState.MouseWheelDelta			= 0;
	mouseState.Position					= XMFLOAT2( 0.0f, 0.0f );
	mouseState.PositionDelta			= XMFLOAT2( 0.0f, 0.0f );

	oldMouseState = mouseState;
}//InitMouse()

void InputManager::Impl::InitFingers() {
	int index = 0;

	for ( auto& finger : fingerState ) {
		finger.ID				= 0;
		finger.isInContact		= false;
		finger.Position			= XMFLOAT2( 0.0f, 0.0f );
		finger.PositionDelta	= XMFLOAT2( 0.0f, 0.0f );
		oldFingerState[index]	= finger;
		index++;
	}
}//InitFingers()

void InputManager::Impl::SetFingerState( InputAction action, PointerPoint ^pointer ) {
	auto dipFactor = DisplayProperties::LogicalDpi / 96.0f; //To convert from DIPs (device independent pixels) to screen resolution pixels.

	auto id			= pointer->PointerId;
	auto properties = pointer->Properties;

	XMFLOAT2 position = XMFLOAT2(
		pointer->Position.X * dipFactor,
		pointer->Position.Y * dipFactor );

#if _DEBUG
	//Utility::WriteDebugMessage(L"Input Position: %f, %f\n", position.x, position.y);
#endif

	int index = 0;

	for ( auto& finger : fingerState ) {
		oldFingerState[index] = finger;
		index++;

		if ( action == InputAction::Pressed ) {
			if ( !finger.isInContact ) {
				finger.ID				= static_cast<std::uint8_t>( id );
				finger.isInContact		= true;
				finger.Position			= position;
				finger.PositionDelta.x	= 0.0f;
				finger.PositionDelta.y	= 0.0f;
				break;
			}
		} else if ( action == InputAction::Moved ) {
			if ( finger.ID == id && finger.isInContact ) {
				auto vOldPos	= XMLoadFloat2( &finger.Position );
				auto vCurrPos	= XMLoadFloat2( &position );
				auto vDeltaPos	= XMVectorSubtract( vCurrPos, vOldPos );

				XMStoreFloat2( &finger.Position, vCurrPos );
				XMStoreFloat2( &finger.PositionDelta, vDeltaPos );
				break;
			}
		} else {
			if ( finger.ID == id ) {
				finger.Position			= position;
				finger.PositionDelta.x	= 0.0f;
				finger.PositionDelta.y	= 0.0f;
				finger.isInContact		= false;
				break;
			}
		}
	}
}//SetFingerState()

void InputManager::Impl::SetMouseState( PointerPoint ^pointer ) {
	auto dipFactor = DisplayProperties::LogicalDpi / 96.0f; //To convert from DIPs (device independent pixels) to screen resolution pixels.
	
	auto id			= pointer->PointerId;
	auto properties = pointer->Properties;

	oldMouseState = mouseState;

	mouseState.ID			= static_cast<std::uint8_t>( id );
	mouseState.Position.x	= pointer->Position.X * dipFactor;
	mouseState.Position.y	= pointer->Position.Y * dipFactor;

#if _DEBUG
	//Utility::WriteDebugMessage(L"Input Position: %f, %f\n", mouseState.Position.x, mouseState.Position.y);
#endif

	mouseState.isLeftButtonPressed		= properties->IsLeftButtonPressed;
	mouseState.isRightButtonPressed		= properties->IsRightButtonPressed;
	mouseState.isMiddleButtonPressed	= properties->IsMiddleButtonPressed;
	mouseState.MouseWheelDelta			= properties->MouseWheelDelta;

}//SetMouseState()

void InputManager::Impl::SetMouseDelta( MouseEventArgs ^args ) {
	//auto dipFactor = DisplayProperties::LogicalDpi / 96.0f; //To convert from DIPs (device independent pixels) to screen resolution pixels.
	//XMFLOAT2 deltaPos = XMFLOAT2(
	//	static_cast<float>(),// * dipFactor,
	//	static_cast<float>(args->MouseDelta.Y));// * dipFactor);

	mouseState.PositionDelta.x += static_cast<float>( args->MouseDelta.X );
	mouseState.PositionDelta.y += static_cast<float>( args->MouseDelta.Y );
}//SetMouseDelta()

InputManager::InputManager() :
	pImpl( new Impl() ) {
}//Ctor()

InputManager::~InputManager() {
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( InputManager );

void InputManager::Initialize() {
	pImpl->InitMouse();
	pImpl->InitFingers();
}//Initialize()

void InputManager::Update() {
	pImpl->mouseState.PositionDelta.x	= 0.0f;
	pImpl->mouseState.PositionDelta.y	= 0.0f;
	pImpl->oldMouseState				= pImpl->mouseState;

	int index = 0;

	for ( auto& finger : pImpl->fingerState ) {
		pImpl->oldFingerState[index] = finger;
		index++;
	}
}//Update()

DirectX::XMVECTOR InputManager::GetFingerPosition( FingerIndex finger ) const {
	return XMLoadFloat2( &pImpl->fingerState[finger].Position );
}//GetMousePosition()

DirectX::XMVECTOR InputManager::GetFingerPositionDelta( FingerIndex finger ) const {
	return XMLoadFloat2( &pImpl->fingerState[finger].PositionDelta );
}//GetMousePositionDelta()

DirectX::XMVECTOR InputManager::GetMousePosition() const {
	return XMLoadFloat2( &pImpl->mouseState.Position );
}//GetMousePosition()

DirectX::XMVECTOR InputManager::GetMousePositionDelta() const {
	return XMLoadFloat2( &pImpl->mouseState.PositionDelta );
}//GetMousePositionDelta()

int InputManager::GetMouseWheelDelta() const {
	return pImpl->mouseState.MouseWheelDelta;
}//GetMouseWheelDelta()

bool InputManager::IsFingerInContact( FingerIndex finger ) const {
	return pImpl->fingerState[finger].isInContact;
}//IsFingerInContact()

bool InputManager::IsLeftMouseButtonPressed() const {
	return pImpl->mouseState.isLeftButtonPressed;
}//IsLeftMouseButtonPressed()

bool InputManager::IsRightMouseButtonPressed() const {
	return pImpl->mouseState.isRightButtonPressed;
}//IsRightMouseButtonPressed()

bool InputManager::IsMiddleMouseButtonPressed() const {
	return pImpl->mouseState.isMiddleButtonPressed;
}//IsMiddleMouseButtonPressed()

void InputManager::ShowMousePointer() {
	if ( !Windows::UI::Core::CoreWindow::GetForCurrentThread()->PointerCursor ) {
		Windows::UI::Core::CoreWindow::GetForCurrentThread()->PointerCursor = 
			ref new Windows::UI::Core::CoreCursor( Windows::UI::Core::CoreCursorType::Arrow, 0 );
	}
}//ShowMousePointer()

void InputManager::HideMousePointer() {
	Windows::UI::Core::CoreWindow::GetForCurrentThread()->PointerCursor = nullptr;
}//HideMousePointer()

void InputManager::SetPointerAction( InputAction action, Windows::UI::Core::PointerEventArgs ^args ) {
	auto pointer = args->CurrentPoint;

	if ( action == InputAction::WheelChanged ) {
		pImpl->SetMouseState( pointer );
	} else {
		switch ( pointer->PointerDevice->PointerDeviceType ) {
			case PointerDeviceType::Touch:
				pImpl->SetFingerState( action, pointer );
				break;
			default:
				pImpl->SetMouseState( pointer );
				break;
		}
	}
}//SetPointerAction()

void InputManager::SetMouseDelta( Windows::Devices::Input::MouseEventArgs ^args ) {
	pImpl->SetMouseDelta( args );
}//SetMouseDelta()

bool InputManager::IsFingerInContactOnce( FingerIndex finger ) const {
	return pImpl->fingerState[finger].isInContact && !pImpl->oldFingerState[finger].isInContact;
}//IsFingerInContactOnce()

bool InputManager::IsLeftMouseButtonPressedOnce() const {
	return pImpl->mouseState.isLeftButtonPressed && !pImpl->oldMouseState.isLeftButtonPressed;
}//IsLeftMouseButtonPressedOnce()

bool InputManager::IsMiddleMouseButtonPressedOnce() const {
	return pImpl->mouseState.isMiddleButtonPressed && !pImpl->oldMouseState.isMiddleButtonPressed;
}//IsMiddleMouseButtonPressedOnce()

bool InputManager::IsRightMouseButtonPressedOnce() const {
	return pImpl->mouseState.isRightButtonPressed && !pImpl->oldMouseState.isRightButtonPressed;
}//IsRightMouseButtonPressedOnce()

bool InputManager::IsFingerNotInContactOnce( FingerIndex finger ) const {
	return !pImpl->fingerState[finger].isInContact && pImpl->oldFingerState[finger].isInContact;
}//IsFingerNotInContactOnce()

bool InputManager::IsLeftMouseButtonReleasedOnce() const {
	return !pImpl->mouseState.isLeftButtonPressed && pImpl->oldMouseState.isLeftButtonPressed;
}//IsLeftMouseButtonReleasedOnce()

bool InputManager::IsRightMouseButtonReleasedOnce() const {
	return !pImpl->mouseState.isRightButtonPressed && pImpl->oldMouseState.isRightButtonPressed;
}//IsRightMouseButtonReleasedOnce()

bool InputManager::IsMiddleMouseButtonReleasedOnce() const {
	return !pImpl->mouseState.isMiddleButtonPressed && pImpl->oldMouseState.isMiddleButtonPressed;
}//IsMiddleMouseButtonReleasedOnce()
