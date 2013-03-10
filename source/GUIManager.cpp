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
#include "GUIManager.h"
#include "Globals.h"
#include "GUIObject.h"

using namespace BreakIt;
using namespace BreakIt::GUI;
using namespace BreakIt::Objects;
using namespace DirectX;
using namespace WinGame;
using namespace WinGame::Graphics;
using namespace WinGame::Audio;
using namespace WinGame::Content;
using namespace WinGame::Input;
using namespace WinGame::Game;

struct StaticText {
	XMFLOAT2		Position;
	XMFLOAT4		Color;
	std::wstring	Text;
};//StaticText object

struct Button {
	XMFLOAT2 Position;
	XMFLOAT4 Color;
	XMFLOAT4 HighlightColor;
	XMFLOAT2 Size;

	bool IsHighlighted;

	std::wstring Text;

	OnClicked ClickedEvent;
};//Button object

class GUIManager::Impl {
public:
	Impl();

	//Vars
	bool isWidescreen;
	RECT blankSourceRect;
	
	//Resources
	std::shared_ptr<Texture2D>	styleTexture;
	std::unique_ptr<GUIObject>	splitterStone;
	std::shared_ptr<SpriteFont> basicFont;
	std::shared_ptr<SpriteFont> gameFont;

	std::vector<std::unique_ptr<StaticText>>	staticText;
	std::vector<std::unique_ptr<Button>>		buttons;

	std::unique_ptr<GUIObject> frame;
	std::unique_ptr<GUIObject> frameEX;

	//Methods
	void DrawSplitter( SpriteBatch *batch );
	void DrawStaticText( SpriteBatch *batch );
	void DrawButtons( SpriteBatch *batch );
	void DrawSides( SpriteBatch *batch );

};//Impl class

GUIManager::Impl::Impl() :
	isWidescreen( false ),
	styleTexture( nullptr ),
	splitterStone( nullptr ) {
}//Ctor()

void GUIManager::Impl::DrawButtons( SpriteBatch *batch ) {
	XMVECTOR pos;

	for ( auto &button : buttons ) {
		//Draw Text
		pos			= XMLoadFloat2( &button->Position );
		auto pos2	= XMVectorAdd( pos, XMVectorSet( -5.0f, 5.0f, 0.0f, 1.0f ) );

		gameFont->DrawString(
			batch,
			button->Text.c_str(),
			pos2,
			Colors::Black
			);

		gameFont->DrawString(
			batch,
			button->Text.c_str(),
			pos,
			button->IsHighlighted ? XMLoadFloat4( &button->HighlightColor ) : XMLoadFloat4( &button->Color )
			);
	}
}//DrawButtons()

void GUIManager::Impl::DrawStaticText( SpriteBatch *batch ) {
	for ( auto &text : staticText ) {
		gameFont->DrawString(
			batch, 
			text->Text.c_str(), 
			XMLoadFloat2( &text->Position ), 
			XMLoadFloat4( &text->Color ),
			0.0f,
			XMVectorZero(),
			XMVectorSet( 0.5f, 0.5f, 0.0f, 1.0f )
			);
	}
}//DrawStaticText()

void GUIManager::Impl::DrawSplitter( SpriteBatch *batch ) {
	std::uint8_t	stoneCount	= isWidescreen ? VirtualWideScreenHeight / 24 : VirtualScreenHeight / 24;
	float			offsetX		= ( ItemTextureWidth - ItemWidth ) * 0.5f;
	float			offsetY		= ( ItemTextureHeight - ItemHeight ) * 0.5f;

	//Left Side
	splitterStone->Position.x = isWidescreen ? SidebarWidth - offsetX : -offsetX;
	for ( decltype( stoneCount ) i = 0; i < stoneCount; ++i ) {
		splitterStone->Position.y = i * ItemHeight - offsetY;
		splitterStone->Draw( batch );
	}

	//Right Side
	splitterStone->Position.x = -SidebarWidth - SplitterWidth - offsetX;
	splitterStone->Position.x += isWidescreen ? VirtualWideScreenWidth : VirtualScreenWidth;

	for ( decltype( stoneCount ) i = 0; i < stoneCount; ++i ) {
		splitterStone->Position.y = i * ItemHeight - offsetY;
		splitterStone->Draw( batch );
	}
}//DrawSplitter()

void GUIManager::Impl::DrawSides( SpriteBatch* batch ) {
	//Draw Frames
	frame->Position.y = 0.0f;

	if ( isWidescreen ) {
		frame->Position.x = 0.0f;
		frame->Draw( batch );
	}

	frame->Position.x = isWidescreen ? VirtualWideScreenWidth - SidebarWidth : VirtualScreenWidth - SidebarWidth;
	frame->Draw( batch );

	//Draw Extension Frames
	if ( !isWidescreen ) {
		frameEX->Position.y = 720.0f;
		frameEX->Position.x = VirtualScreenWidth - SidebarWidth;
		frameEX->Draw( batch );
	}

}//DrawSides()

GUIManager::GUIManager() :
	pImpl( new Impl() ) {
}//Ctor()

GUIManager::~GUIManager() {
	pImpl = nullptr;
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( GUIManager );

void GUIManager::Initialize( const std::shared_ptr<WinGame::Graphics::Texture2D> &texture, const std::shared_ptr<SpriteFont> &gameFont, const std::shared_ptr<SpriteFont> &basicFont, bool widescreen ) {
	pImpl->isWidescreen = widescreen;

	if ( pImpl->styleTexture ) {
		pImpl->styleTexture = nullptr;
	}

	pImpl->styleTexture = texture;

	if ( pImpl->basicFont ) {
		pImpl->basicFont = nullptr;
	}

	pImpl->basicFont = basicFont;
	
	if ( pImpl->gameFont ) {
		pImpl->gameFont = nullptr;
	}

	pImpl->gameFont = gameFont;

	pImpl->splitterStone = std::make_unique<GUIObject>(
		0.0f,
		0.0f,
		ItemTextureWidth,
		ItemTextureHeight,
		texture
		);

	pImpl->splitterStone->SourceRect.left	= 258;
	pImpl->splitterStone->SourceRect.top	= 258;
	pImpl->splitterStone->SourceRect.right	= pImpl->splitterStone->SourceRect.left + Utility::ftoi( ItemTextureWidth );
	pImpl->splitterStone->SourceRect.bottom = pImpl->splitterStone->SourceRect.top + Utility::ftoi( ItemTextureHeight );
	//XMStoreFloat4(&pImpl->splitterStone->RenderColor, XMVectorSet(0.913f, 0.705f, 0.443f, 1.0f));

	//Blank Source RECT
	pImpl->blankSourceRect.left		= 293;
	pImpl->blankSourceRect.top		= 259;
	pImpl->blankSourceRect.right	= pImpl->blankSourceRect.left;
	pImpl->blankSourceRect.bottom	= pImpl->blankSourceRect.top;

	//Init Frames
	pImpl->frame = std::make_unique<GUIObject>(
		0.0f,
		0.0f,
		SidebarWidth,
		720.0f,
		texture
		);

	pImpl->frame->SourceRect.left	= 0;
	pImpl->frame->SourceRect.top	= 258;
	pImpl->frame->SourceRect.right	= pImpl->frame->SourceRect.left + 256;
	pImpl->frame->SourceRect.bottom = pImpl->frame->SourceRect.top + 720;
	XMStoreFloat4( &pImpl->frame->RenderColor, Colors::Gray );

	//Init FrameEX
	pImpl->frameEX = std::make_unique<GUIObject>(
		0.0f,
		0.0f,
		SidebarWidth,
		48.0f,
		texture
		);

	pImpl->frameEX->SourceRect.left		= 514;
	pImpl->frameEX->SourceRect.top		= 0;
	pImpl->frameEX->SourceRect.right	= pImpl->frameEX->SourceRect.left + 256;
	pImpl->frameEX->SourceRect.bottom	= pImpl->frameEX->SourceRect.top + 48;
	XMStoreFloat4( &pImpl->frameEX->RenderColor, Colors::Gray );

	//Clear Collections
	ClearAll();

	Resize( widescreen );
}//Initialize()

void GUIManager::ClearAll() {
	ClearButtons();
	ClearStaticText();
}//ClearAll()

void GUIManager::Resize( bool widescreen ) {
	if ( pImpl->isWidescreen != widescreen ) {
		if ( pImpl->isWidescreen && !widescreen ) {
		} else if ( !pImpl->isWidescreen && widescreen ) {
		}

		pImpl->isWidescreen = widescreen;
	}
}//Resize()

void GUIManager::AddButton( float x, float y, const wchar_t *text, const OnClicked &clicked ) {
	auto obj = std::make_unique<Button>();

	obj->Text			= text;
	obj->IsHighlighted	= false;
	obj->Position		= XMFLOAT2( x, y );
	obj->ClickedEvent	= clicked;

	XMStoreFloat2( &obj->Size, pImpl->gameFont->MeasureString( text ) );
	XMStoreFloat4( &obj->Color, Colors::CornflowerBlue );
	XMStoreFloat4( &obj->HighlightColor, Colors::AliceBlue );

	pImpl->buttons.push_back( std::move( obj ) );
}//AddButton()

void GUIManager::ClearButtons() {
	pImpl->buttons.clear();
}//ClearButtons()

void GUIManager::AddStaticText( float x, float y, DirectX::FXMVECTOR color, const wchar_t *text ) {
	auto obj		= std::make_unique<StaticText>();
	obj->Text		= text;
	obj->Position	= XMFLOAT2( x, y );
	XMStoreFloat4( &obj->Color, color );

	pImpl->staticText.push_back( std::move( obj ) );
}//AddStaticText()

void GUIManager::ClearStaticText() {
	pImpl->staticText.clear();
}//ClearStaticText()

void GUIManager::Update( GameManager *gameManager ) {
	auto input		= gameManager->GetInputManager();
	auto graphics	= gameManager->GetGraphicsManager();
	auto vr			= gameManager->GetVirtualResolution();

	bool		clicked;
	XMVECTOR	pointerPos;

	if ( input->IsFingerInContact() || input->IsFingerNotInContactOnce() ) {
		pointerPos	= vr->Unproject( graphics, input->GetFingerPosition() );
		clicked		= input->IsFingerNotInContactOnce();
	} else {
		pointerPos	= vr->Unproject( graphics, input->GetMousePosition() );
		clicked		= input->IsLeftMouseButtonReleasedOnce();
	}

	XMVECTOR buttonPos;
	XMVECTOR buttonSize;

	for ( auto &button : pImpl->buttons ) {
		buttonPos	= XMLoadFloat2( &button->Position );
		buttonSize	= XMVectorAdd( buttonPos, XMLoadFloat2( &button->Size ) );

		if ( XMVector2LessOrEqual( pointerPos, buttonSize ) && XMVector2GreaterOrEqual( pointerPos, buttonPos ) ) {
			button->IsHighlighted = true;

			if ( clicked ) {
				button->ClickedEvent();
			}
		} else {
			button->IsHighlighted = false;
		}
	}
}//Update()

void GUIManager::Draw( SpriteBatch *batch, bool drawSplitters ) {
	if ( drawSplitters ) {
		pImpl->DrawSplitter( batch );
		pImpl->DrawSides( batch );
	}

	pImpl->DrawStaticText( batch );
	pImpl->DrawButtons( batch );
}//Draw(batch)

void GUIManager::DrawLine( SpriteBatch *batch, float x1, float y1, float x2, float y2, float width, FXMVECTOR color ) {
	auto	p1		= XMVectorSet( x1, y1, 0.0f, 0.0f );
	auto	p2		= XMVectorSet( x2, y2, 0.0f, 0.0f );
	float	angle	= atan2f( y2 - y1, x2 - x1 );
	auto	scale	= XMVector2Length( XMVectorSubtract( p1, p2 ) );
	
	RECT dest = {
		Utility::ftoi( x1 ),
		Utility::ftoi( y1 ),
		Utility::ftoi( x1 + XMVectorGetX( scale ) ),
		Utility::ftoi( y1 + width )
	};

	batch->Draw(
		pImpl->styleTexture->GetResourceView(),
		dest, 
		&pImpl->blankSourceRect, 
		color, 
		angle
		);
}//DrawLine()

void GUIManager::DrawRectangle( SpriteBatch *batch, float x, float y, float width, float height, FXMVECTOR color ) {
	RECT dest = {
		Utility::ftoi( x ),
		Utility::ftoi( y ),
		Utility::ftoi( x + width ),
		Utility::ftoi( y + height )
	};

	batch->Draw(
		pImpl->styleTexture->GetResourceView(),
		dest,
		&pImpl->blankSourceRect,
		color
		);
}//DrawRectangle()

void GUIManager::DrawText( SpriteBatch *batch, float r, float g, float b, float x, float y, const wchar_t *format, ... ) {
	if ( pImpl->basicFont ) {
		va_list args;
		va_start( args, format );
		wchar_t message[1024];
		vswprintf_s( message, 1024, format, args );
		pImpl->basicFont->DrawString( batch, message, XMVectorSet( x, y, 0.0f, 1.0f ), XMVectorSet( r, g, b, 1.0f ) );
	}
}//DrawText()

void GUIManager::DrawGameText( SpriteBatch *batch, float r, float g, float b, float x, float y, float size, const wchar_t *format, ... ) {
	if ( pImpl->basicFont ) {
		va_list args;
		va_start( args, format );
		wchar_t message[1024];
		vswprintf_s( message, 1024, format, args );
		pImpl->gameFont->DrawString( batch, message, XMVectorSet( x, y, 0.0f, 1.0f ), XMVectorSet( r, g, b, 1.0f ), 0.0f, XMVectorZero(), XMVectorSet( size, size, 0.0f, 1.0f ) );
	}
}//DrawText()

float GUIManager::GetGameTextWidth( const wchar_t *text ) {
	XMFLOAT2 size;
	XMStoreFloat2( &size, pImpl->gameFont->MeasureString( text ) );
	return size.x;
}//GetGameTextWidth()
