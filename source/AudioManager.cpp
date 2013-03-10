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
#include "AudioManager.h"

using namespace WinGame;
using namespace WinGame::Audio;
using namespace Microsoft::WRL;
using namespace Windows::ApplicationModel;

class AudioManager::Impl {
public:
	Impl();

	bool isInitialized;

	ComPtr<IXAudio2>		soundEngine;
	ComPtr<IXAudio2>		musicEngine;
	IXAudio2MasteringVoice	*soundMasteringVoice; //NOTE: We only get softpointers since soundEngine->Release() will destroy all used Voices.
	IXAudio2MasteringVoice	*musicMasteringVoice;

	void CreateDeviceIndependentResources();
};//AudioManager::Impl class

AudioManager::Impl::Impl() :
	isInitialized( false ) {
}//Ctor()

void AudioManager::Impl::CreateDeviceIndependentResources() {
	//Create Sound Engine
	Utility::ThrowIfFailed(
		XAudio2Create(
			&soundEngine, 
			0, 
			XAUDIO2_DEFAULT_PROCESSOR
			)
		);

	//Create Music Engine
	Utility::ThrowIfFailed(
		XAudio2Create(
			&musicEngine,
			0,
			XAUDIO2_DEFAULT_PROCESSOR
			)
		);

#ifdef _DEBUG
	//Set Debug Information
	XAUDIO2_DEBUG_CONFIGURATION debugConfig = {0};

	debugConfig.BreakMask = XAUDIO2_LOG_ERRORS;
	debugConfig.TraceMask = XAUDIO2_LOG_ERRORS;

	soundEngine->SetDebugConfiguration( &debugConfig );
	musicEngine->SetDebugConfiguration( &debugConfig );
#endif

	//Create Mastering Voices
	Utility::ThrowIfFailed(
		soundEngine->CreateMasteringVoice( &soundMasteringVoice )
		);

	Utility::ThrowIfFailed(
		musicEngine->CreateMasteringVoice( &musicMasteringVoice )
		);

	this->isInitialized = true;
}//CreateDeviceIndependentResources()

AudioManager::AudioManager() :
	pImpl( new Impl() ) {
}//Ctor()

AudioManager::~AudioManager() {
	pImpl = nullptr;
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( AudioManager );

void AudioManager::Initialize() {
	pImpl->CreateDeviceIndependentResources();
}//Initialize()

void AudioManager::Suspend() {
	if ( pImpl->isInitialized ) {
		pImpl->soundEngine->StopEngine();
		pImpl->musicEngine->StopEngine();
	}
}//Suspend()

void AudioManager::Resume() {
	if ( pImpl->isInitialized ) {
		Utility::ThrowIfFailed( pImpl->soundEngine->StartEngine() );
		Utility::ThrowIfFailed( pImpl->musicEngine->StartEngine() );
	}
}//Resume()

std::shared_ptr<Music> AudioManager::CreateMusic( const wchar_t *filename ) const {
	if ( !pImpl->isInitialized ) {
		return nullptr;
	}

	Utility::ThrowIfFailed( MFStartup( MF_VERSION ) );

	auto path	= Platform::String::Concat( Package::Current->InstalledLocation->Path, "\\" );
	auto music	= std::make_shared<Music>();

	Utility::ThrowIfFailed(
		MFCreateSourceReaderFromURL(
			Platform::String::Concat( path, ref new Platform::String( filename ) )->Data(),
			nullptr,
			&music->reader
			)
		);

	//Set the decoded output format as PCM
	//Note: XAudio2 on Windows can process PCM and ADPCM-encoded buffers.
	ComPtr<IMFMediaType> mediaType;

	Utility::ThrowIfFailed(
		MFCreateMediaType( &mediaType )
		);

	Utility::ThrowIfFailed(
		mediaType->SetGUID( MF_MT_MAJOR_TYPE, MFMediaType_Audio )
		);

	Utility::ThrowIfFailed(
		mediaType->SetGUID( MF_MT_SUBTYPE, MFAudioFormat_PCM )
		);

	Utility::ThrowIfFailed(
		music->reader->SetCurrentMediaType(
			static_cast<std::uint32_t>( MF_SOURCE_READER_FIRST_AUDIO_STREAM ), 
			0, 
			mediaType.Get()
			)
		);

	//Get the Complete WAVEFORMAT from the Media Type
	ComPtr<IMFMediaType> outputMediaType;

	Utility::ThrowIfFailed(
		music->reader->GetCurrentMediaType(
			static_cast<std::uint32_t>( MF_SOURCE_READER_FIRST_AUDIO_STREAM ),
			&outputMediaType
			)
		);

	UINT32			size = 0;
	WAVEFORMATEX	*waveFormat;

	Utility::ThrowIfFailed(
		MFCreateWaveFormatExFromMFMediaType( outputMediaType.Get(), &waveFormat, &size )
		);

	//TODO: Maybe we don't need that part! v

	//Get the total length of th Stream in bytes
	//PROPVARIANT propVariant;

	//Utility::ThrowIfFailed(
	//	music->reader->GetPresentationAttribute(
	//		static_cast<std::uint32_t>(MF_SOURCE_READER_MEDIASOURCE),
	//		MF_PD_DURATION,
	//		&propVariant
	//		)
	//	);

	//LONGLONG duration = propVariant.uhVal.QuadPart;
	//std::uint32_t maxStreamLengthInBytes;

	//double durationInSeconds = (duration / static_cast<double>(10000 * 1000));
	//maxStreamLengthInBytes = static_cast<std::uint32_t>(durationInSeconds * waveFormat->nAvgBytesPerSec);
	
	//Create Music
	Utility::ThrowIfFailed(
		pImpl->musicEngine->CreateSourceVoice( &music->sourceVoice, waveFormat )
		);

	CoTaskMemFree( waveFormat );
	music->isInitialized = true;
	return music;
}//CreateMusic()

std::shared_ptr<Sound> AudioManager::CreateSound( const wchar_t *filename ) const {
	if ( !pImpl->isInitialized ) {
		return nullptr;
	}

	Utility::ThrowIfFailed( MFStartup( MF_VERSION ) );

	auto path = Platform::String::Concat( Package::Current->InstalledLocation->Path, "\\" );
	ComPtr<IMFSourceReader> reader;

	Utility::ThrowIfFailed(
		MFCreateSourceReaderFromURL(
			Platform::String::Concat( path, ref new Platform::String( filename ) )->Data(),
			nullptr,
			&reader
			)
		);

	//Set the decoded output format as PCM
	//Note: XAudio2 on Windows can process PCM and ADPCM-encoded buffers.
	ComPtr<IMFMediaType> mediaType;
	Utility::ThrowIfFailed(
		MFCreateMediaType( &mediaType )
		);

	Utility::ThrowIfFailed(
		mediaType->SetGUID( MF_MT_MAJOR_TYPE, MFMediaType_Audio )
		);

	Utility::ThrowIfFailed(
		mediaType->SetGUID( MF_MT_SUBTYPE, MFAudioFormat_PCM )
		);

	Utility::ThrowIfFailed(
		reader->SetCurrentMediaType(
			static_cast<std::uint32_t>( MF_SOURCE_READER_FIRST_AUDIO_STREAM ), 
			0, 
			mediaType.Get()
			)
		);

	//Get the Complete WAVEFORMAT from the Media Type
	ComPtr<IMFMediaType> outputMediaType;
	Utility::ThrowIfFailed(
		reader->GetCurrentMediaType(
			static_cast<std::uint32_t>( MF_SOURCE_READER_FIRST_AUDIO_STREAM ),
			&outputMediaType
			)
		);

	UINT32			size = 0;
	WAVEFORMATEX	*waveFormat;

	Utility::ThrowIfFailed(
		MFCreateWaveFormatExFromMFMediaType( outputMediaType.Get(), &waveFormat, &size )
		);

	//Get the total length of th Stream in bytes
	PROPVARIANT propVariant;

	Utility::ThrowIfFailed(
		reader->GetPresentationAttribute(
			static_cast<std::uint32_t>(	MF_SOURCE_READER_MEDIASOURCE ),
			MF_PD_DURATION,
			&propVariant
			)
		);

	LONGLONG		duration = propVariant.uhVal.QuadPart;
	std::uint32_t	maxStreamLengthInBytes;

	double durationInSeconds	= ( duration / static_cast<double>( 10000 * 1000 ) );
	maxStreamLengthInBytes		= static_cast<std::uint32_t>( durationInSeconds * waveFormat->nAvgBytesPerSec );

	//make length a multilple of 4 bytes
	maxStreamLengthInBytes = ( maxStreamLengthInBytes + 3 ) / 4 * 4;
	Platform::Array<byte> ^fileData = ref new Platform::Array<byte>( maxStreamLengthInBytes );

	ComPtr<IMFSample>		sample;
	ComPtr<IMFMediaBuffer>	mediaBuffer;
	DWORD					flags = 0;

	int		positionInData	= 0;
	bool	done			= false;

	while ( !done ) {
		Utility::ThrowIfFailed(
			reader->ReadSample( static_cast<std::uint32_t>( MF_SOURCE_READER_FIRST_AUDIO_STREAM ), 0, nullptr, &flags, nullptr, &sample )
			);

		if ( sample ) {
			Utility::ThrowIfFailed(
				sample->ConvertToContiguousBuffer( &mediaBuffer )
				);

			BYTE	*audioData			= nullptr;
			DWORD	sampleBufferLength	= 0;

			Utility::ThrowIfFailed(
				mediaBuffer->Lock( &audioData, nullptr, &sampleBufferLength )
				);

			for ( DWORD i = 0; i < sampleBufferLength; ++i ) {
				fileData[positionInData++] = audioData[i];
			}
		}

		if ( flags & MF_SOURCE_READERF_ENDOFSTREAM ) {
			done = true;
		}
	}

	auto sound = std::make_shared<Sound>();

	//Fix up the array size on match the actual length.
	Platform::Array<byte> ^realFileData = ref new Platform::Array<byte>( ( positionInData + 3 ) / 4 * 4 );

	memcpy( realFileData->Data, fileData->Data, positionInData );
	sound->data = realFileData;

	//Create Sound
	Utility::ThrowIfFailed(
		pImpl->soundEngine->CreateSourceVoice( &sound->sourceVoice, waveFormat )
		);

	CoTaskMemFree( waveFormat );
	sound->isInitialized = true;
	return sound;
}//CreateSound()
