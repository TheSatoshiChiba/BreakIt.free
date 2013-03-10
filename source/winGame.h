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

#include "GameManager.h"

namespace WinGame
{
	ref class WinGameView sealed : public Windows::ApplicationModel::Core::IFrameworkView {
	public:
		WinGameView();

		virtual void Initialize( Windows::ApplicationModel::Core::CoreApplicationView ^applicationView );
		virtual void SetWindow( Windows::UI::Core::CoreWindow ^window );
		virtual void Load( Platform::String ^entryPoint );
		virtual void Run();
		virtual void Uninitialize();

	private:
		bool isWindowClosed;
		bool isWindowVisible;
		bool isWindowInFocus;

		std::unique_ptr<Game::GameManager> gameManager;

		void OnActivated(
			Windows::ApplicationModel::Core::CoreApplicationView ^applicationView,
			Windows::ApplicationModel::Activation::IActivatedEventArgs ^args
			);

		void OnWindowActivationChanged(
			Windows::UI::Core::CoreWindow ^sender, 
			Windows::UI::Core::WindowActivatedEventArgs ^args
			);

		void OnSuspending(
			Platform::Object ^sender, 
			Windows::ApplicationModel::SuspendingEventArgs ^args
			);

		void OnResuming(
			Platform::Object ^sender, 
			Platform::Object ^args
			);

		void OnWindowSizeChanged(
			Windows::UI::Core::CoreWindow ^sender,
			Windows::UI::Core::WindowSizeChangedEventArgs ^args
			);

		void OnWindowClosed(
			Windows::UI::Core::CoreWindow ^sender,
			Windows::UI::Core::CoreWindowEventArgs ^args
			);

		void OnVisibilityChanged(
			Windows::UI::Core::CoreWindow ^sender,
			Windows::UI::Core::VisibilityChangedEventArgs ^args
			);

		void OnLogicalDpiChanged(
			Platform::Object ^sender
			);

		void OnPointerPressed(
			Windows::UI::Core::CoreWindow ^sender, 
			Windows::UI::Core::PointerEventArgs ^args
			);

		void OnPointerReleased(
			Windows::UI::Core::CoreWindow ^sender, 
			Windows::UI::Core::PointerEventArgs ^args
			);

		void OnPointerMoved(
			Windows::UI::Core::CoreWindow ^sender, 
			Windows::UI::Core::PointerEventArgs ^args
			);

		void OnPointerWheelChanged(
			Windows::UI::Core::CoreWindow ^sender, 
			Windows::UI::Core::PointerEventArgs ^args
			);

		void OnMouseMoved(
			Windows::Devices::Input::MouseDevice ^sender, 
			Windows::Devices::Input::MouseEventArgs ^args
			);
	};//WinGameView class

	ref class WinGameViewFactory sealed : Windows::ApplicationModel::Core::IFrameworkViewSource {
	public:
		virtual Windows::ApplicationModel::Core::IFrameworkView ^ CreateView();

	};//WinGameViewFactory class
}//WinGame namespace
