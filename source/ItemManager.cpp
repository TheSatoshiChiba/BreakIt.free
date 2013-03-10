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
#include "ItemManager.h"
#include "BallManager.h"
#include "Globals.h"
#include "Coin.h"
#include "Heart.h"
#include "Diamond.h"
#include "FirstAid.h"
#include "DeathBall.h"
#include "ExtraBall.h"
#include "SteelWall.h"
#include "LaserGun.h"
#include "InvisBall.h"
#include "SoftBall.h"
#include "PadGrow.h"
#include "PadShrink.h"

using namespace BreakIt;
using namespace BreakIt::Objects;
using namespace WinGame;
using namespace WinGame::Graphics;
using namespace DirectX;

struct ItemAnimation {
	std::vector<RECT>	FrameRects;
	std::uint32_t		FrameCounter;
	std::uint32_t		MaximumFrames;
	float				TimeCounter;
	float				TimePerFrame;

	ItemAnimation( std::uint32_t frames, float time ) :
		FrameCounter( 0 ),
		MaximumFrames( frames ),
		TimeCounter( 0.0f ),
		TimePerFrame( time ),
		FrameRects( 0 ) {
	}//Ctor()

};//ItemAnimation strcut

struct ItemSpawnChance {
	double		Weight;
	ITEM_TYPES	Type;

	ItemSpawnChance( ITEM_TYPES type, double weight ) :
		Weight( weight ),
		Type( type ) {
	}//Ctor()

	bool operator<( const ItemSpawnChance &rhs ) const {
		return Weight < rhs.Weight;
	}
};//ItemSpawnChance struct

class ItemManager::Impl {
public:
	Impl();

	bool isWidescreen;

	//Resources
	std::shared_ptr<Texture2D>									itemTexture;
	std::map<ITEM_TYPES, std::unique_ptr<Item>>					staticItems;
	std::map<ITEM_TYPES, std::unique_ptr<ItemAnimation>>		animation;
	std::map<ITEM_TYPES, std::vector<std::unique_ptr<Item>>>	items;

	//Random Vars
	std::default_random_engine				engine;
	std::uniform_real_distribution<double>	distribution;
	std::vector<ItemSpawnChance>			spawnChance;
	double									maximumWeight;

	//Methods
	void AddAnimation( ITEM_TYPES type, std::uint32_t frames, float time, LONG left, LONG top );
	void AddStaticItems();
	void InitRandom();

	std::unique_ptr<Item> CreateItem( float x, float y, ITEM_TYPES type );

	template<typename T>
	void AddStatic( ITEM_TYPES type ) {
		staticItems[type] = std::make_unique<T>(
			0.0f,
			0.0f,
			itemTexture
			);
	}//AddStatic<>()

};//ItemManager::Impl class

ItemManager::Impl::Impl() :
	isWidescreen( false ),
	itemTexture( nullptr ),
	staticItems(),
	animation(),
	items() {
}//Ctor()

std::unique_ptr<Item> ItemManager::Impl::CreateItem( float x, float y, ITEM_TYPES type ) {
	switch ( type ) {
	case ITEM_TYPES::COIN:
		return std::make_unique<Coin>(
			x,
			y,
			itemTexture
			);
	case ITEM_TYPES::HEART:
		return std::make_unique<Heart>(
			x,
			y,
			itemTexture
			);
	case ITEM_TYPES::DIAMOND:
		return std::make_unique<Diamond>(
			x,
			y,
			itemTexture
			);
	case ITEM_TYPES::FIRST_AID:
		return std::make_unique<FirstAid>(
			x,
			y,
			itemTexture
			);
	case ITEM_TYPES::DEATH_BALL:
		return std::make_unique<DeathBall>(
			x,
			y,
			itemTexture
			);
	case ITEM_TYPES::EXTRA_BALL:
		return std::make_unique<ExtraBall>(
			x,
			y,
			itemTexture
			);
	case ITEM_TYPES::STEEL_WALL:
		return std::make_unique<SteelWall>(
			x,
			y,
			itemTexture
			);
	case ITEM_TYPES::LASER_GUN:
		return std::make_unique<LaserGun>(
			x,
			y,
			itemTexture
			);
	case ITEM_TYPES::INVISIBLE_BALL:
		return std::make_unique<InvisBall>(
			x,
			y,
			itemTexture
			);
	case ITEM_TYPES::SOFT_BALL:
		return std::make_unique<SoftBall>(
			x,
			y,
			itemTexture
			);
	case ITEM_TYPES::PADDLE_ENLARGE:
		return std::make_unique<PadGrow>(
			x,
			y,
			itemTexture
			);
	case ITEM_TYPES::PADDLE_SHRINK:
		return std::make_unique<PadShrink>(
			x,
			y,
			itemTexture
			);
	default:
		return nullptr;
	}
}//CreateItem()

void ItemManager::Impl::InitRandom() {
	spawnChance.clear();
	spawnChance.push_back( ItemSpawnChance( ITEM_TYPES::COIN, 400.0 ) );
	spawnChance.push_back( ItemSpawnChance( ITEM_TYPES::HEART, 2.0 ) );
	spawnChance.push_back( ItemSpawnChance( ITEM_TYPES::DIAMOND, 0.05 ) );
	spawnChance.push_back( ItemSpawnChance( ITEM_TYPES::FIRST_AID, 0.09 ) );
	spawnChance.push_back( ItemSpawnChance( ITEM_TYPES::DEATH_BALL, 7.0 ) );
	spawnChance.push_back( ItemSpawnChance( ITEM_TYPES::EXTRA_BALL, 8.0 ) );
	spawnChance.push_back( ItemSpawnChance( ITEM_TYPES::STEEL_WALL, 6.0 ) );
	spawnChance.push_back( ItemSpawnChance( ITEM_TYPES::LASER_GUN, 10.0 ) );
	spawnChance.push_back( ItemSpawnChance( ITEM_TYPES::INVISIBLE_BALL, 0.9 ) );
	spawnChance.push_back( ItemSpawnChance( ITEM_TYPES::SOFT_BALL, 11.0 ) );
	spawnChance.push_back( ItemSpawnChance( ITEM_TYPES::PADDLE_ENLARGE, 12.0 ) );
	spawnChance.push_back( ItemSpawnChance( ITEM_TYPES::PADDLE_SHRINK, 11.5 ) );
	std::sort( std::begin( spawnChance ), std::end( spawnChance ) );

	maximumWeight = 0.0;
	for ( const auto &chance : spawnChance ) {
		maximumWeight += chance.Weight;
	}

	_SYSTEMTIME time;
	GetSystemTime( &time );
	engine.seed( time.wHour + time.wMinute + time.wSecond + time.wMilliseconds );
	distribution = std::uniform_real_distribution<double>( 0.0, maximumWeight - 1.0 );
}//InitRandom()

void ItemManager::Impl::AddStaticItems() {
	AddStatic<Coin>( ITEM_TYPES::COIN );
	AddStatic<Heart>( ITEM_TYPES::HEART );
	AddStatic<Diamond>( ITEM_TYPES::DIAMOND );
	AddStatic<FirstAid>( ITEM_TYPES::FIRST_AID );
	AddStatic<DeathBall>( ITEM_TYPES::DEATH_BALL );
	AddStatic<ExtraBall>( ITEM_TYPES::EXTRA_BALL );
	AddStatic<SteelWall>( ITEM_TYPES::STEEL_WALL );
	AddStatic<LaserGun>( ITEM_TYPES::LASER_GUN );
	AddStatic<InvisBall>( ITEM_TYPES::INVISIBLE_BALL );
	AddStatic<SoftBall>( ITEM_TYPES::SOFT_BALL );
	AddStatic<PadGrow>( ITEM_TYPES::PADDLE_ENLARGE );
	AddStatic<PadShrink>( ITEM_TYPES::PADDLE_SHRINK );
}//AddStaticItems()

void ItemManager::Impl::AddAnimation( ITEM_TYPES type, std::uint32_t frames, float time, LONG left, LONG top ) {
	auto ani = std::make_unique<ItemAnimation>( frames, time );
	RECT sourceRect;

	sourceRect.top		= top;
	sourceRect.bottom	= sourceRect.top + Utility::ftoi( ItemTextureHeight );
	
	for ( std::uint32_t i = 0; i < ani->MaximumFrames; ++i ) {
		sourceRect.left		= i * Utility::ftoi( ItemTextureWidth ) + left;
		sourceRect.right	= sourceRect.left + Utility::ftoi( ItemTextureWidth );
		ani->FrameRects.push_back( sourceRect );
	}

	if ( animation[type] ) {
		animation[type] = nullptr;
	}

	animation[type] = std::move( ani );
}//AddAnimation()

ItemManager::ItemManager() :
	pImpl( new Impl() ) {
}//Ctor()

ItemManager::~ItemManager() {
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( ItemManager );

void ItemManager::Initialize( const std::shared_ptr<Texture2D> &texture, bool widescreen ) {
	pImpl->isWidescreen = widescreen;

	if ( pImpl->itemTexture ) {
		pImpl->itemTexture = nullptr;
	}

	pImpl->itemTexture = texture;
	Clear(); //Clear Everything

	//Add Animations
	pImpl->animation.clear();
	pImpl->AddAnimation( ITEM_TYPES::COIN, 8, 0.08f, 0, 0 );
	pImpl->AddAnimation( ITEM_TYPES::HEART, 4, 0.08f, 0, 102 );
	pImpl->AddAnimation( ITEM_TYPES::DEATH_BALL, 1, 0.0f, 0, 188 );
	pImpl->AddAnimation( ITEM_TYPES::DIAMOND, 3, 0.08f, 0, 136 );
	pImpl->AddAnimation( ITEM_TYPES::EXTRA_BALL, 1, 0.0f, 204, 34 );
	pImpl->AddAnimation( ITEM_TYPES::FIRST_AID, 1, 0.0f, 74, 170 );
	pImpl->AddAnimation( ITEM_TYPES::INVISIBLE_BALL, 1, 0.0f, 196, 68 );
	pImpl->AddAnimation( ITEM_TYPES::LASER_GUN, 1, 0.0f, 238, 34 );
	pImpl->AddAnimation( ITEM_TYPES::PADDLE_ENLARGE, 1, 0.0f, 258, 0 );
	pImpl->AddAnimation( ITEM_TYPES::PADDLE_SHRINK, 1, 0.0f, 68, 204 );
	pImpl->AddAnimation( ITEM_TYPES::SOFT_BALL, 1, 0.0f, 292, 0 );
	pImpl->AddAnimation( ITEM_TYPES::STEEL_WALL, 1, 0.0f, 196, 102 );

	//Add Static Items
	pImpl->staticItems.clear();
	pImpl->AddStaticItems();

	//Init Everything else
	pImpl->InitRandom();
	Resize( widescreen );
}//Initialize()

void ItemManager::Clear() {
	pImpl->items[ITEM_TYPES::COIN].clear();
	pImpl->items[ITEM_TYPES::HEART].clear();
	pImpl->items[ITEM_TYPES::DIAMOND].clear();
	pImpl->items[ITEM_TYPES::FIRST_AID].clear();
	pImpl->items[ITEM_TYPES::DEATH_BALL].clear();
	pImpl->items[ITEM_TYPES::EXTRA_BALL].clear();
	pImpl->items[ITEM_TYPES::STEEL_WALL].clear();
	pImpl->items[ITEM_TYPES::LASER_GUN].clear();
	pImpl->items[ITEM_TYPES::INVISIBLE_BALL].clear();
	pImpl->items[ITEM_TYPES::SOFT_BALL].clear();
	pImpl->items[ITEM_TYPES::PADDLE_ENLARGE].clear();
	pImpl->items[ITEM_TYPES::PADDLE_SHRINK].clear();
}//Clear()

void ItemManager::Resize( bool widescreen ) {
	if ( pImpl->isWidescreen != widescreen ) {
		if ( pImpl->isWidescreen && !widescreen ) {
			for ( auto &map : pImpl->items ) {
				for ( auto &item : map.second ) {
					item->Position.x -= SidebarWidth;
				}
			}
		} else if ( !pImpl->isWidescreen && widescreen ) {
			for ( auto &map : pImpl->items ) {
				for ( auto &item : map.second ) {
					item->Position.x += SidebarWidth;
				}
			}
		}

		pImpl->isWidescreen = widescreen;
	}
}//Resize()

void ItemManager::Animate( float elapsedTime ) {
	for ( auto &map : pImpl->animation ) {
		map.second->TimeCounter += elapsedTime;

		if ( map.second->TimeCounter >= map.second->TimePerFrame ) {
			map.second->TimeCounter = 0.0f;
			++map.second->FrameCounter;

			if ( map.second->FrameCounter >= map.second->MaximumFrames ) {
				map.second->FrameCounter = 0;
			}
		}
	}
}//Animate()

void ItemManager::Update( Player *player, BallManager *balls, SoundManager *sounds, float elapsedTime, float totalTime ) {
	bool hit = false;

	//Coin* coin = nullptr;
	//Heart* heart = nullptr;

	for ( auto &map : pImpl->items ) {
		hit = false;

		for ( auto &item : map.second ) {
			item->Update( elapsedTime, totalTime );

			if ( player->IsTouching( item.get() ) ) {
				sounds->PlayItemSound( map.first );
				item->ApplyEffect( player, balls );

				item->IsVisible = false;
				hit				= true;
			} else if ( item->Position.y > GameFieldHeight + 100.0f ) {
				item->IsVisible = false;
				hit				= true;
			}
		}

		if ( hit ) {
			map.second.erase(
				std::remove_if(
					std::begin( map.second ),
					std::end( map.second ),
					Item::IsRemoveReady
					),
				std::end( map.second )
				);
		}
	}
}//Update()

void ItemManager::Draw( SpriteBatch *batch ) {
	ItemAnimation* ani = nullptr;

	for ( auto &map : pImpl->items ) {
		ani = pImpl->animation[map.first].get();

		for ( auto &item : map.second ) {
			item->DrawAnimated( batch, ani->FrameRects[ani->FrameCounter] );
		}
	}
}//Draw()

void ItemManager::DrawStatic( SpriteBatch *batch, ITEM_TYPES type, float x, float y ) {
	auto ani	= pImpl->animation[type].get();
	auto item	= pImpl->staticItems[type].get();

	item->Position.x = x;
	item->Position.y = y;
	item->DrawAnimated( batch, ani->FrameRects[ani->FrameCounter] );
}//Draw()

void ItemManager::GiveAll( Player *player, BallManager *balls ) {
	for ( auto &map : pImpl->items ) {
		for ( auto &item : map.second ) {
			item->ApplyEffect( player, balls );
		}
	}

	Clear();
}//GiveAll()

void ItemManager::AddItem( float x, float y ) {
	auto rnd = pImpl->distribution( pImpl->engine );
	decltype( rnd ) weight = 0.0;

	for ( const auto &chance : pImpl->spawnChance ) {
		weight += chance.Weight;

		if ( weight > rnd ) {
			pImpl->items[chance.Type].push_back(
				std::move(
					pImpl->CreateItem(
						x, 
						y, 
						chance.Type
						)
					)
				);
			break;
		}
	}
}//AddItem()
