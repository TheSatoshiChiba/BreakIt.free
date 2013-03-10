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

// -----------------------------------------------------------------
// Only a handfull of stuff is really platform independent and almost
// everything has to be turned on at your demand. Some options may
// be forced as default in later verions but i make sure not to break old code.
// -----------------------------------------------------------------
// Options:
// #define UTILITY_WINDOWS_RUNTIME			-> use this to enable all the Windows Runtime helpers
// #define UTILITY_DIRECTX11				-> use this to enable all the DirectX11 helpers
// #define UTILITY_NO_STANDARD_MAKE_UNIQUE	-> use this if you want to have an std::make_unique and it is not implemented yet.
// #define UTILITY_NO_VARIADIC_TEMPLATES	-> use this if your compiler has no support for variadic Templates but you still want to use std::make_unique
//												UTILITY_NO_STANDARD_MAKE_UNIQUE has to be pre-defined before
// -----------------------------------------------------------------
#pragma once

#ifndef UTILITY_H
#define UTILITY_H

#define _UTILITY_NS_START namespace Utility {
#define _UTILITY_NS_END }
#define _UTILITY_STD_NS_START namespace std {
#define _UTILITY_STD_NS_END }

#ifdef UTILITY_WINDOWS_RUNTIME

//Windows Runtime specific includes
#include <wrl\client.h>
#include <ppltasks.h>

using namespace Windows::Storage;
using namespace Concurrency;

#ifdef UTILITY_DIRECTX11

#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#include "ScreenGrab.h"
#include "PrimitiveBatch.h"
#include "GeometricPrimitive.h"
#include "Effects.h"
#include "CommonStates.h"
#include "VertexTypes.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "GJK.h"

#endif

#define UTILITY_THROW_EX( hr ) throw Platform::Exception::CreateException( hr );
#else

//Non WinT specific includes
#include <exception>

#define UTILITY_THROW_EX( hr ) throw std::exception( hr );

#endif

//global Macros
#define UTILITY_CLASS_MOVE( type ) type( type&& ); type& operator=( type&& );
#define UTILITY_CLASS_COPY( type ) type( const type& ); type& operator=( const type& );
#define UTILITY_CLASS_PIMPL() class Impl; std::unique_ptr<Impl> pImpl;
#define _UTILITY_CLASS_PIMPL_MCTOR_IMPL( type ) type::type( type&& other ) : pImpl( std::move( other.pImpl ) ) { other.pImpl = nullptr; }
#define _UTILITY_CLASS_PIMPL_MASSIGN_IMPL( type ) type& type::operator=( type&& other ) { \
	pImpl = nullptr; pImpl = std::move( other.pImpl ); other.pImpl = nullptr; return *this; }
#define UTILITY_CLASS_PIMPL_IMPL( type ) _UTILITY_CLASS_PIMPL_MCTOR_IMPL( type ) _UTILITY_CLASS_PIMPL_MASSIGN_IMPL( type )

#ifdef _DEBUG
#define UTILITY_DEBUG_MSG( msg ) OutputDebugString( msg );
#else
#define UTILITY_DEBUG_MSG( msg )
#endif

//global typedefs
typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;
typedef unsigned long long	ulonglong;

//globals
const float _epsilon = 0.000001f;

_UTILITY_NS_START

	inline void WriteDebugMessage( const wchar_t* format, ... ) {
		va_list args;
		va_start( args, format );
		wchar_t message[1024];
		vswprintf_s( message, 1024, format, args );
		UTILITY_DEBUG_MSG( message );
	}//WriteDebugMessage()

	inline bool NearlyEqual( float left, float right ) {
		return fabsf( left - right ) < _epsilon;
	}//NearlyEqual()

	template<typename T>
	inline bool InRange( T val, T minVal, T maxVal ) {
		return val < maxVal && val >= minVal;
	}//InRange()

	inline int ftoi( float value ) {
		return static_cast<int>( std::floorf( value + 0.5f ) );
	}//ftoi()

#ifdef UTILITY_DIRECTX11

	inline void ThrowIfFailed( _In_ HRESULT hr ) {
		if ( FAILED( hr ) ) {
			UTILITY_THROW_EX( hr )
		}
	}//ThrowIfFailed()

	const float g_dipsPerInch = 96.0f;

	inline float DipsToPixels( float dips, float dpi ) {
		return dips * dpi / g_dipsPerInch;
	}//DipsToPixels()

	inline float PixelToDips( float pixel, float dpi ) {
		return pixel * g_dipsPerInch / dpi;
	}//PixelToDips()
	
#endif //UTILITY_DIRECTX11

#ifdef UTILITY_WINDOWS_RUNTIME

	struct ByteArray {
		Platform::Array<byte>^ data;
	};//ByteArray struct

	//function that writes to a file asynchronously
	inline _Ret_ task<void> WriteDataAsync( _In_ Platform::String^ filename, _In_ ByteArray data ) {
		auto folder			= Windows::ApplicationModel::Package::Current->InstalledLocation;
		auto createFileTask = create_task( folder->CreateFileAsync( filename, CreationCollisionOption::ReplaceExisting ) );

		auto writeBufferTask = createFileTask.then( [&]( StorageFile^ f ) {
			FileIO::WriteBytesAsync( f, data.data );
		} );
	}//WriteDataAsync()

	// function that reads from a binary file asynchronously
	inline _Ret_ task<ByteArray> ReadDataAsync( _In_ Platform::String^ filename ) {
		auto folder			= Windows::ApplicationModel::Package::Current->InstalledLocation;
		auto getFileTask	= create_task( folder->GetFileAsync( filename ) );

		auto readBufferTask = getFileTask.then( []( StorageFile^ f ) {
			return FileIO::ReadBufferAsync( f );
		} );

		auto byteArrayTask = readBufferTask.then( []( Streams::IBuffer^ b ) -> ByteArray {
			auto a = ref new Platform::Array<byte>( b->Length );
			Streams::DataReader::FromBuffer( b )->ReadBytes( a );
			ByteArray ba = { a };
			return ba;
		} );

		return byteArrayTask;
	}//ReadDataAsync()
#endif

_UTILITY_NS_END

_UTILITY_STD_NS_START

#ifdef UTILITY_NO_STANDARD_MAKE_UNIQUE
#ifdef UTILITY_NO_VARIADIC_TEMPLATES
	template<typename T>
	std::unique_ptr<T> make_unique() {
		return std::unique_ptr<T>( new T() );
	}//make_unique()

	template<typename T, typename Arg0>
	std::unique_ptr<T> make_unique( Arg0&& arg0 ) {
		return std::unique_ptr<T>( new T( std::forward<Arg0>( arg0 ) ) );
	}//make_unique()

	template<typename T, typename Arg0, typename Arg1>
	std::unique_ptr<T> make_unique( Arg0&& arg0, Arg1&& arg1 ) {
		return std::unique_ptr<T>( new T( std::forward<Arg0>( arg0 ), std::forward<Arg1>( arg1 ) ) );
	}//make_unique()

	template<typename T, typename Arg0, typename Arg1, typename Arg2>
	std::unique_ptr<T> make_unique( Arg0&& arg0, Arg1&& arg1, Arg2&& arg2 ) {
		return std::unique_ptr<T>( new T( std::forward<Arg0>( arg0 ), std::forward<Arg1>( arg1 ), std::forward<Arg2>( arg2 ) ) );
	}//make_unique()

	template<typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
	std::unique_ptr<T> make_unique( Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3 ) {
		return std::unique_ptr<T>( new T( std::forward<Arg0>( arg0 ), std::forward<Arg1>( arg1 ), std::forward<Arg2>( arg2 ), std::forward<Arg3>( arg3 ) ) );
	}//make_unique()

	template<typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	std::unique_ptr<T> make_unique( Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4 ) {
		return std::unique_ptr<T>( new T( std::forward<Arg0>( arg0 ), std::forward<Arg1>( arg1 ), std::forward<Arg2>( arg2 ), std::forward<Arg3>( arg3 ), std::forward<Arg4>( arg4 ) ) );
	}//make_unique()

	template<typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	std::unique_ptr<T> make_unique( Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5 ) {
		return std::unique_ptr<T>( new T( std::forward<Arg0>( arg0 ), std::forward<Arg1>( arg1 ), std::forward<Arg2>( arg2 ), std::forward<Arg3>( arg3 ),
			std::forward<Arg4>( arg4 ), std::forward<Arg5>( arg5 ) ) );
	}//make_unique()

	template<typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, 
			typename Arg5, typename Arg6>
	std::unique_ptr<T> make_unique( Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, 
		Arg5&& arg5, Arg6&& arg6 )
	{
		return std::unique_ptr<T>( new T( std::forward<Arg0>( arg0 ), std::forward<Arg1>( arg1 ), std::forward<Arg2>( arg2 ), std::forward<Arg3>( arg3 ),
			std::forward<Arg4>( arg4 ), std::forward<Arg5>( arg5 ), std::forward<Arg6>( arg6 ) ) );
	}//make_unique()

	template<typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, 
			typename Arg5, typename Arg6, typename Arg7>
	std::unique_ptr<T> make_unique( Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, 
		Arg5&& arg5, Arg6&& arg6, Arg7&& arg7 ) {
		return std::unique_ptr<T>( new T( std::forward<Arg0>( arg0 ), std::forward<Arg1>( arg1 ), std::forward<Arg2>( arg2 ), std::forward<Arg3>( arg3 ),
			std::forward<Arg4>( arg4 ), std::forward<Arg5>( arg5 ), std::forward<Arg6>( arg6 ), std::forward<Arg7>( arg7 ) ) );
	}//make_unique()

	template<typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, 
			typename Arg5, typename Arg6, typename Arg7, typename Arg8>
	std::unique_ptr<T> make_unique( Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, 
		Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8 ) {
		return std::unique_ptr<T>( new T( std::forward<Arg0>( arg0 ), std::forward<Arg1>( arg1 ), std::forward<Arg2>( arg2 ), std::forward<Arg3>( arg3 ),
			std::forward<Arg4>( arg4 ), std::forward<Arg5>( arg5 ), std::forward<Arg6>( arg6 ), std::forward<Arg7>( arg7 ), std::forward<Arg8>( arg8 ) ) );
	}//make_unique()

	template<typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, 
			typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
	std::unique_ptr<T> make_unique( Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, 
		Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9 ) {
		return std::unique_ptr<T>( new T( std::forward<Arg0>( arg0 ), std::forward<Arg1>( arg1 ), std::forward<Arg2>( arg2 ), std::forward<Arg3>( arg3 ),
			std::forward<Arg4>( arg4 ), std::forward<Arg5>( arg5 ), std::forward<Arg6>( arg6 ), std::forward<Arg7>( arg7 ), std::forward<Arg8>( arg8 ), std::forward<Arg9>( arg9 ) ) );
	}//make_unique()
#else
	template<typename T, typename ...Args>
	std::unique_ptr<T> make_unique( Args&& ...args ) {
		return std::unique_ptr<T>( new T( std::forward<Args>( args )... ) );
	}//make_unique()
#endif	//UTILITY_NO_VARIADIC_TEMPLATES
#endif	//UTILITY_NO_STANDARD_MAKE_UNIQUE

_UTILITY_STD_NS_END

//global includes
#include "BasicTimer.h"

#endif	//define lock
