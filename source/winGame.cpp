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
#include "winGame.h"
#include "LoadingState.h"

using namespace WinGame;
using namespace WinGame::Game;
using namespace WinGame::Graphics;
using namespace WinGame::Input;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Input;
using namespace Windows::Devices::Input;
using namespace Windows::Graphics::Display;
using namespace DirectX;

WinGameView::WinGameView() :
	isWindowClosed( false ),
	isWindowVisible( true ),
	isWindowInFocus( false ) {
}//Ctor()

void WinGameView::Initialize( CoreApplicationView ^applicationView ) {
	applicationView->Activated += 
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>( this, &WinGameView::OnActivated );

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>( this, &WinGameView::OnSuspending );

	CoreApplication::Resuming +=
		ref new EventHandler<Platform::Object^>( this, &WinGameView::OnResuming );

	//Make subclass instances
	gameManager = std::make_unique<GameManager>();
}//Initialize()

void WinGameView::SetWindow( CoreWindow ^window ) {
	//Basic Window Stuff
	window->SizeChanged +=
		ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>( this, &WinGameView::OnWindowSizeChanged );

	window->Closed +=
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>( this, &WinGameView::OnWindowClosed );

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>( this, &WinGameView::OnVisibilityChanged );

	DisplayProperties::LogicalDpiChanged += 
		ref new DisplayPropertiesEventHandler( this, &WinGameView::OnLogicalDpiChanged );

	//Mouse & Touch Stuff
	PointerVisualizationSettings ^visualizationSettings		= PointerVisualizationSettings::GetForCurrentView();
    visualizationSettings->IsContactFeedbackEnabled			= false;
    visualizationSettings->IsBarrelButtonFeedbackEnabled	= false;

	window->PointerPressed +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>( this, &WinGameView::OnPointerPressed );

	window->PointerReleased +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>( this, &WinGameView::OnPointerReleased );

	window->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>( this, &WinGameView::OnPointerMoved );

	window->PointerWheelChanged +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>( this, &WinGameView::OnPointerWheelChanged );

	MouseDevice::GetForCurrentView()->MouseMoved +=
		ref new TypedEventHandler<MouseDevice^, MouseEventArgs^>( this, &WinGameView::OnMouseMoved );

	//Initialize Subclasses
	gameManager->Initialize( window, 1024.0f, 768.0f );
}//SetWindow()

void WinGameView::Load( Platform::String ^entryPoint ) {
	//Load Engine Resources that we need for the first time
	gameManager->PushGameState( GameState::Create<BreakIt::GameStates::LoadingState>() );
}//Load()

void WinGameView::Run() {
	while ( !isWindowClosed ) {
		if ( !gameManager->Run( isWindowVisible ) ) {
			break;
		}
	}

	//gameManager->Suspend();
}//Run()

void WinGameView::Uninitialize() {
}//Uninitialize()

void WinGameView::OnActivated( CoreApplicationView ^applicationView, IActivatedEventArgs ^args ) {
	if( args->PreviousExecutionState == ApplicationExecutionState::Terminated ||
		args->PreviousExecutionState == ApplicationExecutionState::ClosedByUser ) {
		gameManager->isResumed = true;
		gameManager->Resume();
	} else {
		gameManager->isResumed = false;
	}

	CoreWindow::GetForCurrentThread()->Activated +=
		ref new TypedEventHandler<CoreWindow^, WindowActivatedEventArgs^>( this, &WinGameView::OnWindowActivationChanged );
	CoreWindow::GetForCurrentThread()->Activate();
}//OnActivated()

void WinGameView::OnWindowActivationChanged( CoreWindow ^sender, WindowActivatedEventArgs ^args )
{
	auto state = args->WindowActivationState;
	
	if ( state == CoreWindowActivationState::Deactivated ) {
		isWindowInFocus = false;
	}

	if( state == CoreWindowActivationState::CodeActivated ||
		state == CoreWindowActivationState::PointerActivated ) {
		isWindowInFocus = true;
	}
}//OnWindowActivatedChanged()

void WinGameView::OnSuspending( Platform::Object ^sender, SuspendingEventArgs ^args ) {
	SuspendingOperation^ op = args->SuspendingOperation;
	SuspendingDeferral^ deferral = op->GetDeferral();

	gameManager->Suspend();

	deferral->Complete();
}//OnSuspending()

void WinGameView::OnResuming( Platform::Object ^sender, Platform::Object ^args ) {
	gameManager->Resume();
}//OnResuming()

void WinGameView::OnWindowSizeChanged( CoreWindow ^sender, WindowSizeChangedEventArgs ^args ) {
	gameManager->Resize();
}//OnWindowSizeChanged()

void WinGameView::OnWindowClosed( CoreWindow ^sender, CoreWindowEventArgs ^args ) {
	isWindowClosed = true;
}//OnWindowClosed()

void WinGameView::OnVisibilityChanged( CoreWindow ^sender, VisibilityChangedEventArgs ^args ) {
	isWindowVisible = args->Visible;
}//OnVisibilityChanged()

void WinGameView::OnLogicalDpiChanged( Platform::Object ^sender ) {
	gameManager->GetGraphicsManager()->SetDPI( DisplayProperties::LogicalDpi );
}//OnLogicalDpiChanged()

void WinGameView::OnPointerPressed( CoreWindow ^sender, PointerEventArgs ^args ) {
	gameManager->GetInputManager()->SetPointerAction( InputAction::Pressed, args );
	args->Handled = true;
}//OnPointerPressed()

void WinGameView::OnPointerReleased( CoreWindow ^sender, PointerEventArgs ^args ) {
	gameManager->GetInputManager()->SetPointerAction( InputAction::Released, args );
	args->Handled = true;
}//OnPointerReleased()

void WinGameView::OnPointerMoved( CoreWindow ^sender,PointerEventArgs ^args ) {
	gameManager->GetInputManager()->SetPointerAction( InputAction::Moved, args );
	args->Handled = true;
}//OnPointerMoved()

void WinGameView::OnPointerWheelChanged( CoreWindow ^sender, PointerEventArgs ^args ) {
	gameManager->GetInputManager()->SetPointerAction( InputAction::WheelChanged, args );
	args->Handled = true;
}//OnPointerWheelChanged()

void WinGameView::OnMouseMoved( MouseDevice ^sender, MouseEventArgs ^args ) {
	gameManager->GetInputManager()->SetMouseDelta( args );
}//OnMouseMoved()

IFrameworkView ^ WinGameViewFactory::CreateView() {
	return ref new WinGameView();
}//CreateView()
