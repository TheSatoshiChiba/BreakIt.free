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
#include "Music.h"

using namespace WinGame;
using namespace WinGame::Audio;
using namespace Microsoft::WRL;

Music::Music() :
	isInitialized( false ),
	isPlaying( false ),
	currentBuffer( 0 ) {
}//Ctor()

Music::~Music() {
	if ( isInitialized ) {
		Stop();
		sourceVoice->DestroyVoice();

		isInitialized	= false;
		sourceVoice		= nullptr;
	}
}//Dtor()

bool Music::IsInitialized() const {
	return isInitialized;
}//IsInitialized()

bool Music::IsPlaying() const {
	return isPlaying;
}//IsPlaying()

void Music::Play( float volume ) {
	if ( !isInitialized || isPlaying ) {
		return;
	}

	sourceVoice->SetVolume( volume );

	Utility::ThrowIfFailed(
		sourceVoice->Start()
		);

	isPlaying = true;
}//Play()

void Music::Stop() {
	if ( !isInitialized || !isPlaying ) {
		return;
	}

	Utility::ThrowIfFailed(
		sourceVoice->Stop()
		);

	Utility::ThrowIfFailed(
		sourceVoice->FlushSourceBuffers()
		);

	isPlaying = false;
}//Stop()

void Music::Update() {
	if ( !isInitialized || !isPlaying ) {
		return;
	}

	//Find current state of the playing buffers
	XAUDIO2_VOICE_STATE state;
	sourceVoice->GetState( &state );

	//Have any of the buffers completed
	while ( state.BuffersQueued < MaximumBufferCount ) {
		//Retrive the next buffer to stream from MF Music streamer
		data[currentBuffer] = GetNextBuffer();

		if ( data[currentBuffer].size() == 0 ) {
			//Audio file has been fully read, restart the loop
			Restart();
			break;
		}

		//Submit the new Buffer
		XAUDIO2_BUFFER buffer = {0};
		buffer.AudioBytes = static_cast<UINT32>( data[currentBuffer].size() );
		buffer.pAudioData = &data[currentBuffer][0];

		Utility::ThrowIfFailed(
			sourceVoice->SubmitSourceBuffer( &buffer )
			);

		//Advance the Buffer Index
		currentBuffer = ++currentBuffer % MaximumBufferCount;

		//Get the Updated state
		sourceVoice->GetState( &state );
	}
}//Update()

std::vector<byte> Music::GetNextBuffer() {
	std::vector<byte>	resultData;
	ComPtr<IMFSample>	sample;
	DWORD				flags;

	Utility::ThrowIfFailed(
		reader->ReadSample(
			static_cast<DWORD>( MF_SOURCE_READER_FIRST_AUDIO_STREAM ),
			0,
			nullptr,
			&flags,
			nullptr,
			&sample
			)
		);

	//End of Stream
	if ( flags & MF_SOURCE_READERF_ENDOFSTREAM ) {
		return resultData;
	}

	if ( !sample ) {
		throw ref new Platform::FailureException();
	}

	ComPtr<IMFMediaBuffer> mediaBuffer;
	Utility::ThrowIfFailed(
		sample->ConvertToContiguousBuffer( &mediaBuffer )
		);

	std::uint8_t	*audioData			= nullptr;
	std::uint32_t	sampleBufferLength	= 0;

	Utility::ThrowIfFailed(
		mediaBuffer->Lock( &audioData, nullptr, reinterpret_cast<DWORD*>( &sampleBufferLength ) )
		);

	//Prepare and return the new Buffer Data
	resultData.resize( sampleBufferLength );
	CopyMemory( &resultData[0], audioData, sampleBufferLength );

	Utility::ThrowIfFailed(
		mediaBuffer->Unlock()
		);

	return resultData;
}//GetNextBuffer()

void Music::Restart() {
	PROPVARIANT var = {0};
	var.vt			= VT_I8;

	Utility::ThrowIfFailed(
		reader->SetCurrentPosition( GUID_NULL, var )
		);
}//Restart()
