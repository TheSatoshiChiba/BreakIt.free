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
#include "GraphicsManager.h"

using namespace WinGame;
using namespace WinGame::Graphics;
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace DirectX;
using namespace Concurrency;
using namespace Windows::Graphics::Display;

class GraphicsManager::Impl {
public:
	Impl();
	
	float				dpi;
	bool				isInitialized;
	D3D_FEATURE_LEVEL	featureLevel;
	CD3D11_VIEWPORT		deviceViewport;
	XMFLOAT4			clearColor;

	std::unique_ptr<CommonStates> commonStates;

	ComPtr<ID3D11Device1>			d3dDevice;
	ComPtr<IDXGIDevice>				dxgiDevice;
	ComPtr<ID3D11DeviceContext>		d3dContext;
	ComPtr<IDXGISwapChain1>			swapChain;
	ComPtr<ID3D11RenderTargetView>	renderTargetView;
	ComPtr<ID3D11DepthStencilView>	depthStencilView;

	Rect				windowBounds;
	Size				renderTargetSize;
	DisplayOrientations displayOrientation;

	Platform::Agile<CoreWindow> renderWindow;

	void CreateDeviceIndependentResources();
	void CreateDeviceResources();
	void CreateWindowSizeDependentResources();

	Concurrency::task<void> CreateTexture2DAsync(
		const wchar_t *filename, 
		std::shared_ptr<Texture2D> &texture
		) const;

};//Impl class

GraphicsManager::Impl::Impl() :
	isInitialized( false ),
	dpi( -1.0f ) {
	XMStoreFloat4( &clearColor, XMVectorSet( 0.1f, 0.1f, 0.1f, 1.0f ) );
}//Ctor()

void GraphicsManager::Impl::CreateDeviceIndependentResources() {
}//CreateDeviceIndependentResources()

void GraphicsManager::Impl::CreateDeviceResources() {
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	ComPtr<ID3D11Device>		device;
	ComPtr<ID3D11DeviceContext> context;

	Utility::ThrowIfFailed(
		D3D11CreateDevice(
			nullptr, //Use Default Adapter
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr, 
			creationFlags, 
			featureLevels, 
			ARRAYSIZE( featureLevels ), 
			D3D11_SDK_VERSION, 
			&device, 
			&featureLevel, 
			&context
		)
	);

	Utility::ThrowIfFailed( device.As( &d3dDevice ) );
	Utility::ThrowIfFailed( context.As( &d3dContext ) );
	Utility::ThrowIfFailed( d3dDevice.As( &dxgiDevice ) );
	//d3dDevice->CreateDeferredContext1(0, &d3dContext);

	//Common States
	commonStates = std::make_unique<CommonStates>( d3dDevice.Get() );
	
	//Viewports
	deviceViewport	= CD3D11_VIEWPORT();
	swapChain		= nullptr;
}//CreateDeviceResources()

void GraphicsManager::Impl::CreateWindowSizeDependentResources() {
	windowBounds		= renderWindow->Bounds;
	auto windowWidth	= Utility::DipsToPixels( windowBounds.Width, dpi );
	auto windowHeight	= Utility::DipsToPixels( windowBounds.Height, dpi );
	displayOrientation	= DisplayProperties::CurrentOrientation;
	bool swapDimension	= 
		( displayOrientation == DisplayOrientations::Portrait ||
		displayOrientation == DisplayOrientations::PortraitFlipped );

	renderTargetSize.Width	= swapDimension ? windowHeight : windowWidth;
	renderTargetSize.Height = swapDimension ? windowWidth : windowHeight;

	d3dContext->OMSetRenderTargets( 0, nullptr, nullptr );
	renderTargetView = nullptr;
	depthStencilView = nullptr;

	if ( swapChain != nullptr ) {
		Utility::ThrowIfFailed(
			swapChain->ResizeBuffers(
				2, 
				static_cast<UINT>( renderTargetSize.Width ), 
				static_cast<UINT>( renderTargetSize.Height ), 
				DXGI_FORMAT_B8G8R8A8_UNORM, 
				0
				)
			);
	} else {
		d3dContext->Flush();

		//Create Swapchain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

		swapChainDesc.Width					= static_cast<UINT>( renderTargetSize.Width );
		swapChainDesc.Height				= static_cast<UINT>( renderTargetSize.Height );
		swapChainDesc.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.Stereo				= false;
		swapChainDesc.SampleDesc.Count		= 1;
		swapChainDesc.SampleDesc.Quality	= 0;
		swapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount			= 2;
		swapChainDesc.Scaling				= DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags					= 0;

		ComPtr<IDXGIDevice1> device;
		Utility::ThrowIfFailed( d3dDevice.As( &device ) );

		ComPtr<IDXGIAdapter> adapter;
		Utility::ThrowIfFailed( device->GetAdapter( &adapter ) );

		ComPtr<IDXGIFactory2> factory;
		Utility::ThrowIfFailed( adapter->GetParent( IID_PPV_ARGS( &factory ) ) );

		CoreWindow ^window = renderWindow.Get();
		Utility::ThrowIfFailed(
			factory->CreateSwapChainForCoreWindow(
				d3dDevice.Get(), 
				reinterpret_cast<IUnknown*>( window ), 
				&swapChainDesc, 
				nullptr, 
				&swapChain
			)
		);

		Utility::ThrowIfFailed( device->SetMaximumFrameLatency( 1 ) );
	}

	DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;
	switch ( displayOrientation ) {
	case DisplayOrientations::Landscape:
		rotation = DXGI_MODE_ROTATION_IDENTITY;
		//TODO: Set Rotation Matrix
		break;

	case DisplayOrientations::Portrait:
		rotation = DXGI_MODE_ROTATION_ROTATE270;
		//TODO: Set Rotation Matrix
		break;

	case DisplayOrientations::LandscapeFlipped:
		rotation = DXGI_MODE_ROTATION_ROTATE180;
		//TODO: Set Rotation Matrix
		break;

	case DisplayOrientations::PortraitFlipped:
		rotation = DXGI_MODE_ROTATION_ROTATE90;
		//TODO: Set Rotation Matrix
		break;

	default:
		throw ref new Platform::FailureException();
            break;
	}

	Utility::ThrowIfFailed( swapChain->SetRotation( rotation ) );

	//Create RenderTargetView
	ComPtr<ID3D11Texture2D> backBuffer;
	Utility::ThrowIfFailed(
		swapChain->GetBuffer( 0, IID_PPV_ARGS( &backBuffer ) )
		);

	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(
        D3D11_RTV_DIMENSION_TEXTURE2DARRAY,
        DXGI_FORMAT_B8G8R8A8_UNORM,
        0,
        0,
        1
        );

	Utility::ThrowIfFailed(
		d3dDevice->CreateRenderTargetView(
			backBuffer.Get(), 
			&renderTargetViewDesc, 
			&renderTargetView
			)
		);

	//Create DepthStencil view
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT>( renderTargetSize.Width ),
		static_cast<UINT>( renderTargetSize.Height ),
		1,
		1,
		D3D11_BIND_DEPTH_STENCIL
		);
	
	ComPtr<ID3D11Texture2D> depthStencil;
	Utility::ThrowIfFailed(
		d3dDevice->CreateTexture2D(
			&depthStencilDesc,
			nullptr,
			&depthStencil
			)
		);

	auto dep = CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D);
	Utility::ThrowIfFailed(
		d3dDevice->CreateDepthStencilView(
			depthStencil.Get(),
			&dep,
			&depthStencilView
			)
		);

	//Viewports
	deviceViewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		renderTargetSize.Width,
		renderTargetSize.Height
		);

}//CreateWindowSizeDependentResources()

Concurrency::task<void> GraphicsManager::Impl::CreateTexture2DAsync(
	const wchar_t *filename, 
	std::shared_ptr<Texture2D> &texture) const {

	auto obj	= std::make_shared<Texture2D>();
	texture		= obj;

	return create_task( [=]() -> void {
		ComPtr<ID3D11CommandList> command;
		//ComPtr<ID3D11DeviceContext1> context;
		ComPtr<ID3D11Resource> resource;

		//d3dDevice->CreateDeferredContext1(0, &context);

		std::wstring file( filename );
		std::wstring ending = L".dds";

		if ( file.compare( file.length() - ending.length(), ending.length(), ending ) == 0 ) {
			Utility::ThrowIfFailed(
				DirectX::CreateDDSTextureFromFile(
					d3dDevice.Get(),
					filename,
					resource.GetAddressOf(),
					obj->resourceView.GetAddressOf()
				)
			);
		} else {
			Utility::ThrowIfFailed(
				DirectX::CreateWICTextureFromFile(
					d3dDevice.Get(),
					d3dContext.Get(),
					filename,
					resource.GetAddressOf(),
					obj->resourceView.GetAddressOf()
					)
				);
		}

		auto tex = reinterpret_cast<ID3D11Texture2D*>( resource.Get() );
		tex->GetDesc( &obj->description );
		obj->isInitialized = true;

		tex			= nullptr;
		resource	= nullptr;
		//context->FinishCommandList(false, &command);
		//d3dContext->ExecuteCommandList(command.Get(), true);
	} );
}//CreateTexture2DAsync()

GraphicsManager::GraphicsManager() :
	pImpl( new Impl() ) {
}//Ctor()

GraphicsManager::~GraphicsManager() {
	this->pImpl = nullptr;
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( GraphicsManager );

bool GraphicsManager::IsInitialized() const {
	return pImpl->isInitialized;
}//IsInitialized()

float GraphicsManager::GetDPI() const {
	return pImpl->dpi;
}//GetDPI()

float GraphicsManager::GetViewportWidth() const {
	return pImpl->deviceViewport.Width;
}//GetDeviceViewportWidth()

float GraphicsManager::GetViewportHeight() const {
	return pImpl->deviceViewport.Height;
}//GetDeviceViewportHeight()

float GraphicsManager::GetAspectRatio() const {
	return pImpl->deviceViewport.Width / pImpl->deviceViewport.Height;
}//GetAspectRatio()

bool GraphicsManager::IsWidescreen() const {
	return !( pImpl->deviceViewport.Width / pImpl->deviceViewport.Height <= 1.34f );
}//IsWidescreen()

const CommonStates* GraphicsManager::GetCommonStates() const {
	return pImpl->commonStates.get();
}//GetCommonStates()

D3D_FEATURE_LEVEL GraphicsManager::GetFeatureLevel() const {
	return pImpl->featureLevel;
}//GetFeatureLevel()

void GraphicsManager::ResetViewport() {
	pImpl->d3dContext->RSSetViewports( 1, &pImpl->deviceViewport );
}//ResetViewport()

void GraphicsManager::SetViewport( const CD3D11_VIEWPORT &viewport ) {
	pImpl->d3dContext->RSSetViewports( 1, &viewport );
}//SetViewport()

void GraphicsManager::Initialize( CoreWindow ^window, float dpi ) {
	if ( pImpl->isInitialized ) {
		pImpl->swapChain		= nullptr;
		pImpl->renderTargetView = nullptr;
		pImpl->depthStencilView = nullptr;
	}

	pImpl->renderWindow = window;
	pImpl->CreateDeviceIndependentResources();
	pImpl->CreateDeviceResources();
	SetDPI( dpi );

	if ( !pImpl->isInitialized ) {
		pImpl->isInitialized = true;
	}
}//Initialize()

void GraphicsManager::SetClearColor( FXMVECTOR clearColor ) {
	XMStoreFloat4( &pImpl->clearColor, clearColor );
}//SetClearColor()

void GraphicsManager::SetDPI( float dpi ) {
	if ( dpi != pImpl->dpi ) {
		pImpl->dpi = dpi;
		Resize();
	}
}//SetDPI()

void GraphicsManager::Resize() {
	if ( pImpl->dpi != DisplayProperties::LogicalDpi ) {
		return;
	}

	if ( pImpl->renderWindow->Bounds.Width != pImpl->windowBounds.Width ||
		pImpl->renderWindow->Bounds.Height != pImpl->windowBounds.Height ||
		pImpl->displayOrientation != DisplayProperties::CurrentOrientation ) {
		pImpl->CreateWindowSizeDependentResources();
	}
}//Resize()

void GraphicsManager::Clear() {
	auto &context = pImpl->d3dContext;
	//context->Flush();
	
	try {
		FLOAT f[4];
		f[0] = f[1] = f[2] = 0.1f;
		f[3] = 1.0f;

		//auto color = XMLoadFloat4(&pImpl->clearColor);
		context->RSSetViewports( 1, &pImpl->deviceViewport );
		context->ClearRenderTargetView( pImpl->renderTargetView.Get(), f );//color.m128_f32);
		context->ClearDepthStencilView( pImpl->depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );
		context->OMSetRenderTargets( 1, pImpl->renderTargetView.GetAddressOf(), pImpl->depthStencilView.Get() );
	}
	catch ( std::exception& ) {
	}
}//Clear()

void GraphicsManager::Present() {
	HRESULT hr = pImpl->swapChain->Present( 1, 0 );

	if ( hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET ) {
		Initialize( pImpl->renderWindow.Get(), pImpl->dpi );
	} else {
		Utility::ThrowIfFailed( hr );
	}
}//Present()

std::shared_ptr<SpriteBatch> GraphicsManager::CreateSpriteBatch() const {
	auto result = std::make_shared<SpriteBatch>( pImpl->d3dContext.Get() );
	return result;
}//CreateSpriteBatch()

std::shared_ptr<SpriteFont> GraphicsManager::CreateSpriteFont( const wchar_t *filename ) const {
	auto result = std::make_shared<SpriteFont>( pImpl->d3dDevice.Get(), filename );
	return result;
}//CreateSpriteFont()

std::shared_ptr<Texture2D> GraphicsManager::CreateTexture2D( const wchar_t *filename ) const {
	std::shared_ptr<Texture2D> texture;

	pImpl->CreateTexture2DAsync( filename, texture ).then( []( Concurrency::task<void> t ) {
		try {
			t.get();
		} catch( Platform::Exception ^e ) {
			UTILITY_DEBUG_MSG( e->Message->Data() );
			UTILITY_THROW_EX( e->HResult );
		}
	} );

	return texture;
}//CreateTexture2D()
