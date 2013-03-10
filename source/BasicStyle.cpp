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
#include "BasicStyle.h"
#include "Globals.h"

using namespace DirectX;
using namespace BreakIt;
using namespace BreakIt::Styles;

class BasicStyle::Impl {
public:
	Impl();

	std::wstring				styleTexture;
	std::vector<std::wstring>	levels;

	std::wstring SideHit;
	std::wstring BallLost;
	std::wstring PlayerHit;
	std::wstring BrickHit;
	std::wstring BrickBreak;
	std::wstring GameWon;
	std::wstring GameLost;
	std::wstring PointsAdd;
	std::wstring HealthAdd;
	std::wstring NegativeItem;
	std::wstring PositiveItem;
	std::wstring LaserShotSound;

	std::wstring backgroundMusic;
};//Impl class

BasicStyle::Impl::Impl() :
	styleTexture( L"Assets\\Textures\\basicStyle.dds" ),
	backgroundMusic( L"Assets\\Music\\Psykick_Roya_loop_mix_.wma" ),
	SideHit( L"Assets\\Sounds\\SideHit.wav" ),
	BallLost( L"Assets\\Sounds\\BallLost.wav" ),
	PlayerHit( L"Assets\\Sounds\\PlayerHit.wav" ),
	BrickHit( L"Assets\\Sounds\\BrickHit.wav" ),
	BrickBreak( L"Assets\\Sounds\\BrickBreak.wav" ),
	GameWon( L"Assets\\Sounds\\round_end.wav" ),
	GameLost( L"Assets\\Sounds\\death.wav" ),
	PointsAdd( L"Assets\\Sounds\\PointsAdd.wav" ),
	HealthAdd( L"Assets\\Sounds\\HealthAdd.wav" ),
	NegativeItem( L"Assets\\Sounds\\NegativeItem.wav" ),
	PositiveItem( L"Assets\\Sounds\\PositiveItem.wav" ),
	LaserShotSound( L"Assets\\Sounds\\LaserShot.wav" ) {

	//The Level Cycle
	std::wostringstream stream;

	for ( int i = 0; i < 50; ++i ) {
		stream	<< L"Assets\\Maps\\" 
				<< i 
				<< ".bmp";

		levels.push_back( stream.str().c_str() );
		stream.str( L"" );
	}
}//Ctor()

BasicStyle::BasicStyle() :
	pImpl( new Impl() ) {
}//Ctor()

BasicStyle::~BasicStyle() {
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( BasicStyle );

const wchar_t* BasicStyle::GetStyleTexture() const {
	return pImpl->styleTexture.c_str();
}//GetStyleTexture()

std::uint8_t BasicStyle::GetLevelCount() const {
	return static_cast<std::uint8_t>( pImpl->levels.size() );
}//GetLevelCount()

const wchar_t* BasicStyle::GetBackgroundMusic() const {
	return pImpl->backgroundMusic.c_str();
}//GetBackgroundMusic()

const wchar_t* BasicStyle::GetLevelFilename( std::uint8_t index ) const {
	if ( index >= pImpl->levels.size() ) {
		return pImpl->levels.back().c_str();
	}

	return pImpl->levels[index].c_str();
}//GetLevelFilename()

const wchar_t* BasicStyle::GetSideHitSound() const {
	return pImpl->SideHit.c_str();
}

const wchar_t* BasicStyle::GetBallLostSound() const {
	return pImpl->BallLost.c_str();
}

const wchar_t* BasicStyle::GetPlayerHitSound() const {
	return pImpl->PlayerHit.c_str();
}

const wchar_t* BasicStyle::GetBrickHitSound() const {
	return pImpl->BrickHit.c_str();
}

const wchar_t* BasicStyle::GetBrickBreakSound() const {
	return pImpl->BrickBreak.c_str();
}

const wchar_t* BasicStyle::GetGameWonSound() const {
	return pImpl->GameWon.c_str();
}

const wchar_t* BasicStyle::GetGameLostSound() const {
	return pImpl->GameLost.c_str();
}

const wchar_t* BasicStyle::GetPointsAddSound() const {
	return pImpl->PointsAdd.c_str();
}

const wchar_t* BasicStyle::GetHealthAddSound() const {
	return pImpl->HealthAdd.c_str();
}

const wchar_t* BasicStyle::GetPositiveItemSound() const {
	return pImpl->PositiveItem.c_str();
}

const wchar_t* BasicStyle::GetNegativeItemSound() const {
	return pImpl->NegativeItem.c_str();
}

const wchar_t* BasicStyle::GetLaserShotSound() const  {
	return pImpl->LaserShotSound.c_str();
}
