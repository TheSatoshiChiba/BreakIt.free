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

namespace WinGame {
	namespace Audio {
		const std::size_t MaximumBufferCount = 3U;

		class Music {
			friend class AudioManager;

		public:
			explicit Music();
			virtual ~Music();

			bool IsInitialized() const;
			bool IsPlaying() const;

			void Play( float volume = 1.0f );
			void Update();
			void Stop();

		private:
			std::vector<byte> GetNextBuffer();
			void Restart();

			bool isInitialized;
			bool isPlaying;

			Microsoft::WRL::ComPtr<IMFSourceReader> reader;
			IXAudio2SourceVoice						*sourceVoice;
			std::vector<byte>						data[MaximumBufferCount];
			std::size_t								currentBuffer;
			
		};//Music class

	}//Audio namespace
}//WinGame namespace
