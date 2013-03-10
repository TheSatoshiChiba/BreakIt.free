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
#include "BrickManager.h"
#include "IGameObject.h"
#include "Globals.h"
#include "Laser.h"

using namespace DirectX;
using namespace BreakIt;
using namespace BreakIt::Objects;
using namespace WinGame;
using namespace WinGame::Graphics;
using namespace WinGame::Audio;

const std::int32_t MaximumObjects = 16;

enum class BRICK_ADDITION : std::uint8_t {
	ADD_FAILED = 0x00U,
	ADD_SUCCEEDED,
	ADD_OVERFLOW
};//BrickAddition enum class

class BrickTree : public GameObject {
public:
	std::unique_ptr<BrickTree> NorthWest;
	std::unique_ptr<BrickTree> NorthEast;
	std::unique_ptr<BrickTree> SouthWest;
	std::unique_ptr<BrickTree> SouthEast;

	std::vector<std::shared_ptr<Brick>> Bricks;

	BrickTree( float x, float y, float width, float height ) :
		GameObject( x, y, width, height ),
		NorthWest( nullptr ),
		NorthEast( nullptr ),
		SouthWest( nullptr ),
		SouthEast( nullptr ),
		Bricks() {
	}//Ctor()

	virtual ~BrickTree() {
	}//Dtor()

	std::uint32_t GetBrickCount() const {
		std::uint32_t result = static_cast<std::uint32_t>( Bricks.size() );

		if ( NorthWest ) {
			result += NorthWest->GetBrickCount();
			result += NorthEast->GetBrickCount();
			result += SouthWest->GetBrickCount();
			result += SouthEast->GetBrickCount();
		}
		return result;
	}//GetBrickCount()

	BRICK_ADDITION Add( const std::shared_ptr<Brick> &brick ) {
		XMVECTOR depth, normal;
		XMVECTOR size = XMVectorSet( ItemWidth, ItemHeight, 0.0f, 1.0f );

		if ( !IsTouching( brick.get(), depth, normal ) ) {
			return BRICK_ADDITION::ADD_FAILED;
		}

		if ( XMVector2Less( depth, size ) ) {
			return BRICK_ADDITION::ADD_OVERFLOW;
		}

		if ( !NorthWest && Bricks.size() <= MaximumObjects ) {
			Bricks.push_back( brick );
			return BRICK_ADDITION::ADD_SUCCEEDED;
		}

		if ( !NorthWest ) {
			Subdivide();
		}

		BRICK_ADDITION result;
		//XMStoreFloat4(&object->RenderColor, Colors::Red);
		result = NorthWest->Add( brick );
		if ( OverflowCheck( result, brick ) ) {
			return BRICK_ADDITION::ADD_SUCCEEDED;
		}

		//XMStoreFloat4(&object->RenderColor, Colors::Green);
		result = SouthWest->Add( brick );
		if ( OverflowCheck( result, brick ) ) {
			return BRICK_ADDITION::ADD_SUCCEEDED;
		}

		//XMStoreFloat4(&object->RenderColor, Colors::Blue);
		result = NorthEast->Add( brick );
		if ( OverflowCheck( result, brick ) ) {
			return BRICK_ADDITION::ADD_SUCCEEDED;
		}

		//XMStoreFloat4(&object->RenderColor, Colors::Yellow);
		result = SouthEast->Add( brick );
		if ( OverflowCheck( result, brick ) ) {
			return BRICK_ADDITION::ADD_SUCCEEDED;
		}

		return BRICK_ADDITION::ADD_FAILED;
	}//Add()

	void Clear() {
		Bricks.clear();
	
		if ( NorthWest ) {
			NorthWest->Clear();
			NorthEast->Clear();
			SouthWest->Clear();
			SouthEast->Clear();

			NorthWest = nullptr;
			NorthEast = nullptr;
			SouthWest = nullptr;
			SouthEast = nullptr;
		}
	}//Clear()

	void TranslateX( float x ) {
		Position.x += x;

		for ( auto &brick : Bricks ) {
			brick->Position.x += x;
		}

		if ( NorthWest ) {
			NorthWest->TranslateX( x );
			NorthEast->TranslateX( x );
			SouthWest->TranslateX( x );
			SouthEast->TranslateX( x );
		}
	}//TranslateX()

	void Draw( SpriteBatch *batch, const RECT &sourceRECT ) {
		for ( auto &brick : Bricks ) {
			brick->DrawAnimated( batch, sourceRECT );
		}

		if ( NorthWest ) {
			NorthWest->Draw( batch, sourceRECT );
			NorthEast->Draw( batch, sourceRECT );
			SouthWest->Draw( batch, sourceRECT );
			SouthEast->Draw( batch, sourceRECT );
		}
	}//Draw()

	void Subdivide() {
		float halfWidth		= Size.x * 0.5f;
		float halfHeight	= Size.y * 0.5f;

		NorthWest = std::make_unique<BrickTree>( Position.x, Position.y, halfWidth, halfHeight );
		NorthEast = std::make_unique<BrickTree>( Position.x + halfWidth, Position.y, halfWidth, halfHeight );
		SouthWest = std::make_unique<BrickTree>( Position.x, Position.y + halfHeight, halfWidth, halfHeight );
		SouthEast = std::make_unique<BrickTree>( Position.x + halfWidth, Position.y + halfHeight, halfWidth, halfHeight );

		decltype( Bricks ) overflow;
		BRICK_ADDITION result;

		for ( auto &brick : Bricks ) {
			//XMStoreFloat4(&brick->RenderColor, Colors::Red);
			result = NorthWest->Add( brick );

			if ( result == BRICK_ADDITION::ADD_SUCCEEDED ) {
				continue;
			} else if ( result == BRICK_ADDITION::ADD_OVERFLOW ) {
				overflow.push_back( brick );
				continue;
			}

			//XMStoreFloat4(&brick->RenderColor, Colors::Green);
			result = SouthWest->Add( brick );
			if ( result == BRICK_ADDITION::ADD_SUCCEEDED ) {
				continue;
			} else if ( result == BRICK_ADDITION::ADD_OVERFLOW ) {
				overflow.push_back( brick );
				continue;
			}

			//XMStoreFloat4(&brick->RenderColor, Colors::Blue);
			result = NorthEast->Add( brick );
			if ( result == BRICK_ADDITION::ADD_SUCCEEDED ) {
				continue;
			} else if ( result == BRICK_ADDITION::ADD_OVERFLOW ) {
				overflow.push_back( brick );
				continue;
			}

			//XMStoreFloat4(&brick->RenderColor, Colors::Yellow);
			result = SouthEast->Add( brick );
			if ( result == BRICK_ADDITION::ADD_SUCCEEDED ) {
				continue;
			} else if ( result == BRICK_ADDITION::ADD_OVERFLOW ) {
				overflow.push_back( brick );
				continue;
			}
		}

		Bricks.clear();
		Bricks = std::move( overflow );
	}//Subdivide()

	bool OverflowCheck( BRICK_ADDITION addition, const std::shared_ptr<Brick> &object ) {
		switch ( addition ) {
		case BRICK_ADDITION::ADD_OVERFLOW:
			Bricks.push_back( object );
			return true;
		case BRICK_ADDITION::ADD_FAILED:
			return false;
		case BRICK_ADDITION::ADD_SUCCEEDED:
		default:
			return true;
		}
	}//OverflowCheck()

	void CheckCollision( Laser *laser, Player *player, SoundManager *sounds, ItemManager *items ) {
		bool hit = false;

		for ( auto &brick : Bricks ) {
			if ( brick->IsVisible && brick->IsTouching( laser ) ) {
				brick->Health = 0;
				sounds->PlaySound( SOUND_FILE::BRICK_BREAK );
				player->Points += brick->Points;
				items->AddItem( brick->Position.x, brick->Position.y );
				laser->IsVisible = false;
				brick->IsVisible = false;
				hit = true;
			}
		}

		if ( hit ) {
			Bricks.erase(
			std::remove_if(
				std::begin( Bricks ),
				std::end( Bricks ),
				Brick::IsRemoveReady
				),
			std::end( Bricks )
			);
		}

		//Check Tree Collision
		if ( NorthWest ) {
			if ( NorthWest->IsTouching( laser ) ) {
				NorthWest->CheckCollision( laser, player, sounds, items );
			}

			if ( NorthEast->IsTouching( laser ) ) {
				NorthEast->CheckCollision( laser, player, sounds, items );
			}

			if ( SouthWest->IsTouching( laser ) ) {
				SouthWest->CheckCollision( laser, player, sounds, items );
			}

			if ( SouthEast->IsTouching( laser ) ) {
				SouthEast->CheckCollision( laser, player, sounds, items );
			}
		}
	}//CheckCollision()

	XMVECTOR CheckCollision( Ball *ball, Player *player, SoundManager *sounds, ItemManager *items, XMVECTOR &maxDepth, XMVECTOR &maxNormal ) {
		bool hit = false;
		XMVECTOR depth, normal;
		XMVECTOR correction = XMVectorZero();

		for ( auto &brick : Bricks ) {
			if ( brick->IsVisible && brick->IsTouching( ball, depth, normal ) ) {
				correction = XMVectorAdd( correction, XMVectorMultiply( depth, normal ) );

				//NOTE: We get the biggest Depth and the corresponding Normal and reflect the Ball
				//		In only that direction. Even the Edge cases are covered that way i a slightly unelegant matter
				//		but it should be sufficient enough for this game.
				if ( XMVector2Greater( depth, maxDepth ) ) {
					maxDepth	= depth;
					maxNormal	= normal;
				}

				if ( brick->Health != 0	) {
					if ( ball->GetPower() >= brick->Health ) {
						brick->Health = 0;
					} else {
						brick->Health -= ball->GetPower();
					}

					XMStoreFloat4( &brick->RenderColor, Brick::RenderColorFromHealth( brick->Health ) );
				}

				if ( brick->Health == 0 ) {
					sounds->PlaySound( SOUND_FILE::BRICK_BREAK );
					player->Points += brick->Points;
					items->AddItem( brick->Position.x, brick->Position.y );
					brick->IsVisible = false;
					hit = true;
				} else {
					sounds->PlaySound( SOUND_FILE::BRICK_HIT );
				}
			}
		}//foreach(brick)

		if ( hit ) {
			Bricks.erase(
			std::remove_if(
				std::begin( Bricks ),
				std::end( Bricks ),
				Brick::IsRemoveReady
				),
			std::end( Bricks )
			);
		}

		//Check Tree Collision
		if ( NorthWest ) {
			if ( NorthWest->IsTouching( ball ) ) {
				correction = XMVectorAdd( correction, NorthWest->CheckCollision( ball, player, sounds, items, maxDepth, maxNormal ) );
			}

			if ( NorthEast->IsTouching( ball ) ) {
				correction = XMVectorAdd( correction, NorthEast->CheckCollision( ball, player, sounds, items, maxDepth, maxNormal ) );
			}

			if ( SouthWest->IsTouching( ball ) ) {
				correction = XMVectorAdd( correction, SouthWest->CheckCollision( ball, player, sounds, items, maxDepth, maxNormal ) );
			}

			if ( SouthEast->IsTouching( ball ) ) {
				correction = XMVectorAdd( correction, SouthEast->CheckCollision( ball, player, sounds, items, maxDepth, maxNormal ) );
			}
		}

		return correction;
	}//CheckCollision()

};//BrickTree class

class BrickManager::Impl {
public:
	Impl();

	bool isWidescreen;

	std::shared_ptr<Texture2D>	brickTexture;
	std::unique_ptr<Brick>		staticBrick;
	std::unique_ptr<BrickTree>	brickTree;

	std::vector<RECT>	frames;
	std::uint32_t		frame;
	float				time;
	float				animatedTime;
};//BrickManager::Impl class

BrickManager::Impl::Impl() :
	brickTexture( nullptr ),
	isWidescreen( false ),
	frames( 0 ),
	frame( 0 ),
	time( 0.0f ),
	animatedTime( 0.0f ),
	brickTree( nullptr ) {
}//Ctor()

BrickManager::BrickManager() :
	pImpl( new Impl() ) {
}//Ctor()

BrickManager::~BrickManager() {
}
//Dtor()

UTILITY_CLASS_PIMPL_IMPL( BrickManager );

std::uint32_t BrickManager::GetCount() const {
	return pImpl->brickTree->GetBrickCount();
}//GetCount()

void BrickManager::Initialize( const std::shared_ptr<Texture2D> &texture, bool widescreen ) {
	pImpl->isWidescreen = widescreen;

	if ( pImpl->brickTexture ) {
		pImpl->brickTexture = nullptr;
	}

	pImpl->brickTexture = texture;
	pImpl->animatedTime = 0.0f;
	pImpl->frame		= 0;
	pImpl->time			= 0.0f;

	pImpl->frames.clear();

	if ( pImpl->staticBrick ) {
		pImpl->staticBrick = nullptr;
	}

	pImpl->staticBrick = std::make_unique<Brick>(
		0.0f,
		0.0f,
		static_cast<std::uint8_t>( 0U ),
		texture
		);

	RECT brickRECT;
	brickRECT.top		= 68;
	brickRECT.bottom	= brickRECT.top + Utility::ftoi( ItemTextureHeight );

	for ( std::int32_t i = 0; i < 4; ++i ) {
		brickRECT.left	= Utility::ftoi( i * ItemTextureWidth );
		brickRECT.right = brickRECT.left + Utility::ftoi( ItemTextureWidth );

		pImpl->frames.push_back( brickRECT );
	}

	if ( pImpl->brickTree ) {
		pImpl->brickTree->Clear();
		pImpl->brickTree = nullptr;
	}

	pImpl->brickTree = std::make_unique<BrickTree>(
		widescreen ? SplitterWidth + SidebarWidth : SplitterWidth,
		0.0f,
		GameFieldWidth,
		MaximumBrickHeight
		);

	Resize( widescreen );
}//Initialize()

void BrickManager::Resize( bool widescreen ) {
	if ( pImpl->isWidescreen != widescreen ) {
		if ( pImpl->isWidescreen && !widescreen ) {
			pImpl->brickTree->TranslateX( -SidebarWidth );
		} else if ( !pImpl->isWidescreen && widescreen ) {
			pImpl->brickTree->TranslateX( SidebarWidth );
		}

		pImpl->isWidescreen = widescreen;
	}
}//Resize()

void BrickManager::Clear() {
	pImpl->brickTree->Clear();
}//Clear()

void BrickManager::AddBrick( float x, float y, std::uint8_t red, std::uint8_t green, std::uint8_t blue ) {
	auto brick = Brick::CreateBrickFromColor( red, green, blue );

	if ( brick ) {
		brick->Position.x	= x;
		brick->Position.y	= y;
		brick->Texture		= pImpl->brickTexture;

		BRICK_ADDITION result = pImpl->brickTree->Add( brick );
		if ( result == BRICK_ADDITION::ADD_FAILED ) {
			UTILITY_DEBUG_MSG( L"UH-OH!" );
		} else if ( result == BRICK_ADDITION::ADD_OVERFLOW ) {
			pImpl->brickTree->Bricks.push_back( brick );
		}

		brick = nullptr;
	}
}//AddBrick()

void BrickManager::Animate( float elapsedTime ) {
	if ( pImpl->time >= 3.0f ) {
		pImpl->animatedTime += elapsedTime;

		if ( pImpl->animatedTime >= 0.1f ) {
			pImpl->animatedTime -= 0.1f;
			++pImpl->frame;

			if ( pImpl->frame >= pImpl->frames.size() ) {
				pImpl->frame		= 0;
				pImpl->animatedTime = 0.0f;
				pImpl->time			= 0.0f;
			}
		}
	} else {
		pImpl->time += elapsedTime;
	}
}//Animate()

void BrickManager::Draw( SpriteBatch *batch ) {
	pImpl->brickTree->Draw( batch, pImpl->frames[pImpl->frame] );
}//Draw()

void BrickManager::DrawStatic( SpriteBatch *batch, float x, float y, std::uint8_t health ) {
	pImpl->staticBrick->Position.x = x;
	pImpl->staticBrick->Position.y = y;

	XMStoreFloat4( &pImpl->staticBrick->RenderColor, Brick::RenderColorFromHealth( health ) );

	pImpl->staticBrick->DrawAnimated( batch, pImpl->frames[pImpl->frame] );
}//DrawStatic()

void BrickManager::CheckCollision( Ball *ball, Player *player, SoundManager *sounds, ItemManager *items ) {
	if ( pImpl->brickTree->IsTouching( ball ) ) {
		XMVECTOR maxDepth	= XMVectorZero();
		XMVECTOR maxNormal	= XMVectorZero();
		XMVECTOR correction = pImpl->brickTree->CheckCollision( ball, player, sounds, items, maxDepth, maxNormal );

		if ( ball->IsBouncy() ) {
			ball->Bounce( correction, maxDepth, maxNormal );
		}
	}
}//CheckCollision()

void BrickManager::CheckCollision( Laser *shot, Player *player, SoundManager *sounds, ItemManager *items ) {
	if ( pImpl->brickTree->IsTouching( shot ) ) {
		pImpl->brickTree->CheckCollision( shot, player, sounds, items );
	}
}//CheckCollision()
