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

namespace Utility
{
	class BasicTimer final {
	public:
		explicit BasicTimer() {
			if ( !QueryPerformanceFrequency( &m_frequency ) ) {
				UTILITY_THROW_EX( E_FAIL );
			}

			this->Reset();
		}//Ctor()
    
		virtual ~BasicTimer() {
		}//Dtor()

		BasicTimer( BasicTimer&& other ) :
			m_frequency( std::move( other.m_frequency ) ) {
			this->Reset();
		}//MCtor()

		BasicTimer& operator=( BasicTimer&& other ) {
			this->m_frequency = std::move( other.m_frequency );
			this->Reset();
			return *this;
		}//MoveAssign()

		BasicTimer( const BasicTimer& other ) :
			m_frequency( other.m_frequency ) {
			this->Reset();
		}//CCtor()

		BasicTimer& operator=( const BasicTimer& other ) {
			this->m_frequency = other.m_frequency;
			this->Reset();
			return *this;
		}//CopyAssign

		void Reset() {
			Update();
			m_startTime = m_currentTime;
			m_total		= 0.0f;
			m_delta		= 1.0f / 60.0f;
		}//Reset()
    
		void Update() {
			if ( !QueryPerformanceCounter( &m_currentTime ) ) {
				throw ref new Platform::FailureException();
			}
        
			m_total = static_cast<float>(
				static_cast<double>( m_currentTime.QuadPart - m_startTime.QuadPart ) /
				static_cast<double>( m_frequency.QuadPart )
				);
        
			if ( m_lastTime.QuadPart == m_startTime.QuadPart ) {
				// If the timer was just reset, report a time delta equivalent to 60Hz frame time.
				m_delta = 1.0f / 60.0f;
			} else {
				m_delta = static_cast<float>(
					static_cast<double>( m_currentTime.QuadPart-m_lastTime.QuadPart ) /
					static_cast<double>( m_frequency.QuadPart )
					);
			}
        
			m_lastTime = m_currentTime;
		}//Update()
    
		float GetTotalTime() const {
			return m_total;
		}//GetTotalTime()
    
		float GetDeltaTime() const {
			return m_delta;
		}//GetDeltaTime()

	private:

		float			m_total;
		float			m_delta;
		LARGE_INTEGER	m_frequency;
		LARGE_INTEGER	m_currentTime;
		LARGE_INTEGER	m_startTime;
		LARGE_INTEGER	m_lastTime;
		
	};//BasicTimer class

	typedef std::unique_ptr<BasicTimer> UniqueBasicTimer;
}//Utility namespace
