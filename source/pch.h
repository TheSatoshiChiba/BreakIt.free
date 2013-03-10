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

#define WIN32_LEAN_AND_MEAN

#pragma warning(disable : 4100)		// unreferenced formal parameter

//SDL Checks (Code Analysis)
#include <sal.h>

//DirectX Includes
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <wincodec.h>

//XAudio Includes
#include <xaudio2.h>
#include <xaudio2fx.h>

//Media Foundation Includes
#include <mmreg.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>

//Windows Runtime
#include <wrl.h>
#include <agile.h>
#include <collection.h>

//STL includes
#include <map>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <string>
#include <random>
#include <sstream>

//C Includes
#include <cstdint>

//Concurrency (Parallel Patterns Library)
#include <ppl.h>		
#include <ppltasks.h>

//The Utility Header
#define UTILITY_WINDOWS_RUNTIME
#define UTILITY_DIRECTX11
#define UTILITY_NO_STANDARD_MAKE_UNIQUE
#define UTILITY_NO_VARIADIC_TEMPLATES

#include "Utility.h"
//#include "Globals.h"