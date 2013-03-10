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

//Essentials
#include "pch.h"
#include "GameManager.h"
#include "SnappedState.h"
#include "Globals.h"

//Engine Includes
#include "GraphicsManager.h"

//Game Includes
#include "Logo.h"
#include "GUIManager.h"

//Using directives
using namespace DirectX;
using namespace WinGame;
using namespace WinGame::Audio;
using namespace WinGame::Content;
using namespace WinGame::Game;
using namespace WinGame::Graphics;
using namespace WinGame::Input;
using namespace BreakIt;
using namespace BreakIt::GameStates;
using namespace BreakIt::Objects;
using namespace BreakIt::GUI;

class SnappedState::Impl {
public:
	Impl();

	//GameObjects
	std::unique_ptr<Logo>		objLogo;
	std::unique_ptr<GUIManager> guiManager;

	//Methods
	void Initialize( GameManager *manager );
	void Create( GameManager *manager );
	void RenderLogo();
};//SnappedState::Impl class

SnappedState::Impl::Impl() {
}//Ctor()

void SnappedState::Impl::Initialize( GameManager *manager ) {
	auto graphics		= manager->GetGraphicsManager();
	auto screenWidth	= graphics->GetViewportWidth();
	auto screenHeight	= graphics->GetViewportHeight();
	auto content		= manager->GetMenuContent();

	objLogo->Position = XMFLOAT2(
		screenWidth * 0.5f,
		100.0f
		);

	objLogo->AngleDifference = 15.0f;
	objLogo->ScaleDifference = 0.1f;

	auto font = content->LoadFont( graphics, FontFilename.c_str() );
	auto size = font->MeasureString( L"PAUSE" );
	
	guiManager->ClearStaticText();
	guiManager->AddStaticText(
		( screenWidth * 0.5f - XMVectorGetX( size ) * 0.5f ) - 2.0f,
		( screenHeight * 0.5f - XMVectorGetY( size ) * 0.5f ) + 2.0f,
		Colors::Black,
		L"PAUSE"
		);
	guiManager->AddStaticText(
		screenWidth * 0.5f - XMVectorGetX( size ) * 0.5f,
		screenHeight * 0.5f - XMVectorGetY( size ) * 0.5f,
		Colors::CornflowerBlue,
		L"PAUSE"
		);

	guiManager->Resize( graphics->IsWidescreen() );
	graphics->SetClearColor( XMVectorSet( 0.1f, 0.1f, 0.1f, 1.0f ) );
}//Initialize()

void SnappedState::Impl::Create( GameManager *manager ) {
	auto graphics	= manager->GetGraphicsManager();
	auto content	= manager->GetMenuContent();

	//Create Logo
	objLogo = std::make_unique<Logo>(
		0.0f,
		0.0f,
		512.0f,
		256.0f,
		content->LoadTexture2D( graphics, TextureFilename.c_str() )
		);
	objLogo->Origin = XMFLOAT2( 256.0f, 128.0f );
	objLogo->Size = XMFLOAT2( 256.0f, 128.0f );

	//Create Labels
	guiManager = std::make_unique<GUIManager>();
	guiManager->Initialize( nullptr, content->LoadFont( graphics, FontFilename2.c_str() ), content->LoadFont( graphics, FontFilename.c_str() ), graphics->IsWidescreen() );
}//Load()

SnappedState::SnappedState() :
	pImpl( new Impl() ) {
}//Ctor()

SnappedState::~SnappedState() {
	pImpl = nullptr;
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( SnappedState );

void SnappedState::Load( GameManager *manager ) {
	GameState::Load (manager );

	pImpl->Create( manager );
	pImpl->Initialize( manager );
}//Load()

void SnappedState::Unload() {
}//Unload()

void SnappedState::Pause() {
}//Pause()

void SnappedState::Resume() {
	pImpl->Initialize( gameManager );
}//Resume()

void SnappedState::Update( float elapsedTime, float totalTime ) {
	pImpl->objLogo->Update( elapsedTime, totalTime );
}//Update()

void SnappedState::Draw( float elapsedTime, float totalTime ) {
	auto sprites = gameManager->GetSpriteBatch();

	sprites->Begin();
	pImpl->objLogo->Draw( sprites );
	pImpl->guiManager->Draw( sprites );
	sprites->End();
}//Draw()
