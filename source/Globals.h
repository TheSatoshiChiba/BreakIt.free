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

namespace BreakIt
{
	//Resolution vars
	const std::int32_t VirtualScreenWidth		= 1024;
	const std::int32_t VirtualScreenHeight		= 768;
	const std::int32_t VirtualWideScreenWidth	= 1280;
	const std::int32_t VirtualWideScreenHeight	= 720;

	//Game Field Vars
	const float GameFieldWidth		= 720.0f;
	const float GameFieldHeight		= 720.0f;
	const float MaximumBrickHeight	= 480.0f;
	const float SplitterWidth		= 24.0f;
	const float SidebarWidth		= 256.0f;

	const std::int8_t BricksWide	= 30;
	const std::int8_t BricksHeigh	= 20;

	//Ball Vars
	const float BallWidth			= 20.0f;
	const float BallHeight			= 20.0f;
	const float BallTextureWidth	= 32.0f;
	const float BallTextureHeight	= 32.0f; 
	const float MinimumBallSpeed	= 220.0f;

	//Player Vars
	const float PlayerTextureWidth	= 32.0f;
	const float PlayerTextureHeight = 40.0f;
	const float PlayerWidth			= 32.0f;
	const float PlayerHeight		= 40.0f;

	//Item vars
	const float ItemWidth			= 24.0f;
	const float ItemHeight			= 24.0f;
	const float ItemTextureWidth	= 32.0f;
	const float ItemTextureHeight	= 32.0f;
	const float MinimumItemSpeed	= 100.0f;
	const float ItemDuration		= 10.0f;

	const std::int8_t	MaximumBrickHealth	= 5;
	const std::uint8_t	PlayerStartHealth	= 3;

	//GUI
	const float ButtonWidth		= 242.0f;
	const float ButtonHeight	= 64.0f;

	enum class ITEM_TYPES : std::uint8_t {
		COIN,
		HEART,
		DIAMOND,
		FIRST_AID,
		DEATH_BALL,
		EXTRA_BALL,
		STEEL_WALL,
		LASER_GUN,
		INVISIBLE_BALL,
		SOFT_BALL,
		PADDLE_ENLARGE,
		PADDLE_SHRINK
	};//ITEM_TYPES enum class

	//Resource vars
	const std::wstring MusicFilename	= L"Assets\\Music\\Psykick_Roya_loop_mix_.wma";
	const std::wstring TextureFilename	= L"Assets\\Textures\\gui.dds";
	const std::wstring FontFilename		= L"Assets\\Fonts\\segoeUIsemi20.spritefont";
	const std::wstring FontFilename2	= L"Assets\\Fonts\\TinyBoxBlackBitA8.spritefont";

}//BreakIt namespace
