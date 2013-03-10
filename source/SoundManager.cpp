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
#include "SoundManager.h"
#include "Globals.h"

using namespace WinGame;
using namespace WinGame::Audio;
using namespace WinGame::Content;
using namespace WinGame::Game;
using namespace BreakIt;
using namespace BreakIt::Styles;
using namespace BreakIt::Objects;
using namespace DirectX;

class SoundManager::Impl {
public:
	Impl();

	std::map<SOUND_FILE, std::shared_ptr<Sound>> Sounds;
};//SoundManager::Impl class

SoundManager::Impl::Impl() {
}//Ctor()

SoundManager::SoundManager() :
	pImpl( new Impl() ) {
}//Ctor()

SoundManager::~SoundManager() {
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( SoundManager );

void SoundManager::Initialize( GameManager *manager ) {
	auto content	= manager->GetGameplayContent();
	auto audio		= manager->GetAudioManager();
	auto style		= manager->GetStyleManager();

	pImpl->Sounds.clear();

	//Load Audio Content
	pImpl->Sounds[SOUND_FILE::BALL_LOST]		= content->LoadSound( audio, style->GetBallLostSound() );
	pImpl->Sounds[SOUND_FILE::BRICK_BREAK]		= content->LoadSound( audio, style->GetBrickBreakSound() );
	pImpl->Sounds[SOUND_FILE::BRICK_HIT]		= content->LoadSound( audio, style->GetBrickHitSound() );
	pImpl->Sounds[SOUND_FILE::POINTS_ADD]		= content->LoadSound( audio, style->GetPointsAddSound() );
	pImpl->Sounds[SOUND_FILE::GAME_LOST]		= content->LoadSound( audio, style->GetGameLostSound() );
	pImpl->Sounds[SOUND_FILE::GAME_WON]			= content->LoadSound( audio, style->GetGameWonSound() );
	pImpl->Sounds[SOUND_FILE::HEALTH_ADD]		= content->LoadSound( audio, style->GetHealthAddSound() );
	pImpl->Sounds[SOUND_FILE::PLAYER_HIT]		= content->LoadSound( audio, style->GetPlayerHitSound() );
	pImpl->Sounds[SOUND_FILE::SIDE_HIT]			= content->LoadSound( audio, style->GetSideHitSound() );
	pImpl->Sounds[SOUND_FILE::NEGATIVE_ITEM]	= content->LoadSound( audio, style->GetNegativeItemSound() );
	pImpl->Sounds[SOUND_FILE::POSITIVE_ITEM]	= content->LoadSound( audio, style->GetPositiveItemSound() );
	pImpl->Sounds[SOUND_FILE::LASER_SHOT]		= content->LoadSound( audio, style->GetLaserShotSound() );
}//Initialize()

void SoundManager::PlaySound( SOUND_FILE file, float volume ) {
	if ( file == SOUND_FILE::UNKNOWN ) {
		return;
	}

	pImpl->Sounds[file]->Play( volume );
}//PlaySound()

void SoundManager::PlayItemSound( ITEM_TYPES type, float volume ) {
	switch ( type ) {
	case ITEM_TYPES::DIAMOND:
	case ITEM_TYPES::COIN:
		PlaySound( SOUND_FILE::POINTS_ADD, volume );
		break;

	case ITEM_TYPES::FIRST_AID:
	case ITEM_TYPES::HEART:
		PlaySound( SOUND_FILE::HEALTH_ADD, volume );
		break;

	case ITEM_TYPES::DEATH_BALL:
	case ITEM_TYPES::EXTRA_BALL:
	case ITEM_TYPES::LASER_GUN:
	case ITEM_TYPES::PADDLE_ENLARGE:
	case ITEM_TYPES::STEEL_WALL:
		PlaySound( SOUND_FILE::POSITIVE_ITEM, volume );
		break;

	case ITEM_TYPES::INVISIBLE_BALL:
	case ITEM_TYPES::PADDLE_SHRINK:
	case ITEM_TYPES::SOFT_BALL:
		PlaySound( SOUND_FILE::NEGATIVE_ITEM, volume );
		break;

	default:
		PlaySound( SOUND_FILE::UNKNOWN, volume );
		break;
	}
}//PlayItemSound()
