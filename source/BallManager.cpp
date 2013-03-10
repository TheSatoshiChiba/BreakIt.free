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
#include "BallManager.h"
#include "Globals.h"
#include <cmath>

using namespace BreakIt;
using namespace BreakIt::Objects;
using namespace WinGame;
using namespace WinGame::Graphics;
using namespace DirectX;

class BallManager::Impl {
public:
	Impl();

	bool isWidescreen;
	
	std::unique_ptr<Ball>				staticBall;
	std::shared_ptr<Texture2D>			ballTexture;
	std::vector<std::unique_ptr<Ball>>	balls;

	std::vector<std::unique_ptr<GameObject>>	borders;
	std::unique_ptr<DrawableObject>				wall;

	bool			wallActive;
	float			wallTimer;
	std::uint8_t	globalPower;
	float			powerTimer;
	bool			globalVisibility;
	float			visibleTimer;

	//Methods
	void ResetPowerLevel();
	void ResetVisibility();
	void ResetWall();

};//BallManager::Impl class

BallManager::Impl::Impl() :
	staticBall( nullptr ),
	ballTexture( nullptr ),
	balls( 0 ),
	isWidescreen( false ),
	borders( 0 ),
	globalPower( 1 ),
	powerTimer( 0.0f ),
	globalVisibility( true ),
	visibleTimer( 0.0f ),
	wallActive( false ),
	wallTimer( 0.0f ) {
}//Ctor()

BallManager::BallManager() :
	pImpl( new Impl() ) {
}//Ctor()

void BallManager::Impl::ResetWall() {
	wallActive	= false;
	wallTimer	= 0.0f;
}//ResetWall()

void BallManager::Impl::ResetVisibility() {
	globalVisibility	= true;
	visibleTimer		= 0.0f;

	for ( auto &ball : balls ) {
		ball->SetInvisibility( !globalVisibility );
	}
}//ResetVisibility()

void BallManager::Impl::ResetPowerLevel() {
	globalPower = 1;
	powerTimer	= 0.0f;

	for ( auto &ball : balls ) {
		ball->SetPower( globalPower );
	}
}//ResetPowerLevel()

BallManager::~BallManager() {
}//Dtor()

UTILITY_CLASS_PIMPL_IMPL( BallManager );

std::uint32_t BallManager::GetCount() const {
	return static_cast<std::uint32_t>( pImpl->balls.size() );
}//GetCount()

void BallManager::Initialize( const std::shared_ptr<Texture2D> &texture, bool widescreen ) {
	pImpl->isWidescreen = widescreen;

	if ( pImpl->ballTexture ) {
		pImpl->ballTexture = nullptr;
	}

	pImpl->ballTexture = texture;
	pImpl->balls.clear();
	pImpl->ResetPowerLevel();
	pImpl->ResetVisibility();

	if ( pImpl->staticBall ) {
		pImpl->staticBall = nullptr;
	}

	pImpl->staticBall = std::make_unique<Ball>(
		0.0f,
		0.0f,
		texture
		);

	//The Wall
	pImpl->wall = std::make_unique<DrawableObject>(
		0.0f,
		0.0f,
		72.0f,
		24.0f,
		texture
		);

	pImpl->wall->SourceRect.left	= 130;
	pImpl->wall->SourceRect.top		= 34;
	pImpl->wall->SourceRect.right	= pImpl->wall->SourceRect.left + 72;
	pImpl->wall->SourceRect.bottom	= pImpl->wall->SourceRect.top + 24;
	pImpl->wall->Position.x			= pImpl->isWidescreen ? SidebarWidth + SplitterWidth - 10.0f : SplitterWidth -10.0f;
	pImpl->wall->Position.y			= GameFieldHeight - PlayerTextureHeight - 20.0f;
	pImpl->wall->HitBoxSize.y		= 100.0f;
	pImpl->wall->HitBoxSize.x		= GameFieldWidth + 20.0f;

	//The Borders
	pImpl->borders.clear();
	for ( int i = 0; i < 4; ++i ) {
		if ( i > 1 ) {
			//Top & Bottom Border
			pImpl->borders.push_back(
				std::move(
					std::make_unique<GameObject>(
						0.0f,
						i == 3 ? GameFieldHeight : -100.0f,
						GameFieldWidth + SplitterWidth * 2.0f,
						100.0f
						)
					)
				);
		} else {
			//Left + Right border
			pImpl->borders.push_back(
				std::move(
					std::make_unique<GameObject>(
						0.0f,
						-100.0f,
						SidebarWidth + SplitterWidth,
						GameFieldHeight + 200.0f
						)
					)
				);
		}
	}

	pImpl->powerTimer	= 0.0f;
	pImpl->visibleTimer = 0.0f;
	Resize( widescreen );
}//Initialize()

void BallManager::Resize( bool widescreen ) {
	if ( !widescreen ) {
		pImpl->borders[0]->Position.x = -SidebarWidth;
		pImpl->borders[1]->Position.x = SplitterWidth + GameFieldWidth;
		pImpl->borders[2]->Position.x = -SplitterWidth;
		pImpl->borders[3]->Position.x = -SplitterWidth;
	} else {
		float splitterAndBar = SplitterWidth + SidebarWidth;
		float sideNoSplitter = SidebarWidth - SplitterWidth;

		pImpl->borders[0]->Position.x = 0.0f;
		pImpl->borders[1]->Position.x = splitterAndBar + GameFieldWidth;
		pImpl->borders[2]->Position.x = sideNoSplitter;
		pImpl->borders[3]->Position.x = sideNoSplitter;
	}

	if ( pImpl->isWidescreen != widescreen ) {
		//Move Balls
		if ( pImpl->isWidescreen && !widescreen ) {
			for ( auto &ball : pImpl->balls ) {
				ball->Position.x -= SidebarWidth;
			}
		} else if ( !pImpl->isWidescreen && widescreen ) {
			for ( auto &ball : pImpl->balls ) {
				ball->Position.x += SidebarWidth;
			}
		}

		pImpl->isWidescreen = widescreen;
	}

	pImpl->wall->Position.x = pImpl->isWidescreen ? SidebarWidth + SplitterWidth - 10.0f : SplitterWidth -10.0f;
}//Resize()

void BallManager::AddBall() {
	float screenCenterX = GameFieldWidth * 0.5f + SplitterWidth;
	float posY = MaximumBrickHeight + 50.0f;

	if ( pImpl->isWidescreen ) {
		screenCenterX += SidebarWidth;
	}

	AddBall(
		screenCenterX - BallTextureWidth * 0.5f,
		posY
		);
}

void BallManager::AddBall( float x, float y ) {
	auto ball = std::make_unique<Ball>(
		x,
		y,
		pImpl->ballTexture
		);
	ball->SetInvisibility( !pImpl->globalVisibility );
	ball->SetPower( pImpl->globalPower );
	pImpl->balls.push_back( std::move( ball ) );
}//AddBall()

void BallManager::ActivateWall() {
	pImpl->wallActive	= true;
	pImpl->wallTimer	= 0.0f;
}//ActivateWall()

void BallManager::SplitBall() {
	if ( pImpl->balls.size() == 0 ) {
		AddBall();
	}

	auto pos = pImpl->balls[0]->Position;
	auto vel = pImpl->balls[0]->Velocity;

	for ( int i = 0; i < 2; ++i ) {
		auto ball = std::make_unique<Ball>(
			pos.x,
			pos.y,
			pImpl->ballTexture
			);
		ball->SetInvisibility( !pImpl->globalVisibility );
		ball->SetPower( pImpl->globalPower );
		ball->Velocity.x = i == 0 ? vel.x : -vel.x;
		ball->Velocity.y = i == 0 ? -vel.y : vel.y;
		pImpl->balls.push_back( std::move( ball ) );
	}
}//SplitBall()

void BallManager::Clear() {
	pImpl->ResetPowerLevel();
	pImpl->ResetVisibility();
	pImpl->ResetWall();
	pImpl->balls.clear();
}//Clear()

void BallManager::Update( Player *player, SoundManager *sounds, float elapsedTime, float totalTime ) {
	XMVECTOR depth, normal;

	XMVECTOR maxDepth	= XMVectorZero();
	XMVECTOR maxNormal	= XMVectorZero();
	XMVECTOR correction = XMVectorZero();

	bool removeBalls	= false;
	bool bounceBall		= false;

	//Wall Timer
	if ( pImpl->wallActive ) {
		pImpl->wallTimer += elapsedTime;

		if ( pImpl->wallTimer >= ItemDuration ) {
			sounds->PlaySound( SOUND_FILE::NEGATIVE_ITEM );
			pImpl->ResetWall();
		}
	}

	//Visible Timer
	if ( !pImpl->globalVisibility ) {
		pImpl->visibleTimer += elapsedTime;

		if ( pImpl->visibleTimer >= ItemDuration ) {
			sounds->PlaySound( SOUND_FILE::POSITIVE_ITEM );
			pImpl->ResetVisibility();
		}
	}

	//Power Timer
	if ( pImpl->globalPower != 1 ) {
		pImpl->powerTimer += elapsedTime;

		if ( pImpl->powerTimer >= ItemDuration ) {
			if ( pImpl->globalPower == 0 ) {
				sounds->PlaySound( SOUND_FILE::POSITIVE_ITEM );
			} else {
				sounds->PlaySound( SOUND_FILE::NEGATIVE_ITEM );
			}

			pImpl->ResetPowerLevel();
		}
	}

	//Clamp Player
	player->Clamp(
		pImpl->borders[0]->GetHitBoxTopRight(),
		pImpl->borders[1]->GetHitBoxBottomLeft()
		);

	for ( auto &ball : pImpl->balls ) {
		ball->Update( elapsedTime, totalTime );

		bounceBall = false;

		if ( pImpl->wallActive && pImpl->wall->IsTouching( ball.get(), depth, normal ) ) {
			bounceBall = true;
			correction = XMVectorAdd( correction, XMVectorMultiply( depth, normal ) );

			if ( XMVector2Greater( depth, maxDepth ) ) {
				maxDepth	= depth;
				maxNormal	= normal;
			}

			sounds->PlaySound( SOUND_FILE::SIDE_HIT );
		}

		for ( decltype( pImpl->borders.size() ) i = 0; i < pImpl->borders.size(); ++i ) {
			if ( i == 3 && pImpl->borders[i]->IsTouching( ball.get() ) ) {		//Death Zone

				sounds->PlaySound( SOUND_FILE::BALL_LOST );
				//player->Points -= static_cast<std::uint32_t>(static_cast<double>(player->Points) * 0.1);

				if ( player->Points > 5 ) {
					player->Points -= 5;
				} else {
					player->Points = 0;
				}

				if ( player->Health != 0 && pImpl->balls.size() == 1 ) {
					player->Health--;
				}

				ball->IsVisible = false;
				removeBalls		= true;
			} else if ( pImpl->borders[i]->IsTouching( ball.get(), depth, normal ) ) {
				bounceBall = true;
				correction = XMVectorAdd( correction, XMVectorMultiply( depth, normal ) );

				if ( XMVector2Greater( depth, maxDepth ) ) {
					maxDepth	= depth;
					maxNormal	= normal;
				}

				sounds->PlaySound( SOUND_FILE::SIDE_HIT );
			}
		}
		
		if ( bounceBall ) {
			ball->Bounce( correction, maxDepth, maxNormal );
		}

		if ( player->IsTouching( ball.get(), depth, normal ) ) {
			ball->Bounce( player, depth, normal );
			sounds->PlaySound( SOUND_FILE::PLAYER_HIT );
		}
	}

	if ( removeBalls ) {
		pImpl->balls.erase(
		std::remove_if(
			std::begin( pImpl->balls ),
			std::end( pImpl->balls ),
			Ball::IsRemoveReady
			),
		std::end( pImpl->balls )
		);
	}
}//Update()

void BallManager::CheckCollision( Player *player, BrickManager *bricks, ItemManager *items, SoundManager *sounds ) {
	for ( auto &ball : pImpl->balls ) {
		bricks->CheckCollision( ball.get(), player, sounds, items );
	}
}//CheckCollision()

void BallManager::Draw( SpriteBatch *batch ) {
	float offset = pImpl->isWidescreen ? SplitterWidth + SidebarWidth : SplitterWidth;

	//DRAW Walls
	if ( pImpl->wallActive ) {
		for ( int i = 0; i < 10; ++i ) {
			pImpl->wall->Position.x = offset + i * pImpl->wall->Size.x;
			pImpl->wall->Draw( batch );
		}
	}
	pImpl->wall->Position.x = offset - 10.0f;

	//DRAW Balls
	for ( auto &ball : pImpl->balls ) {
		ball->Draw( batch );
	}
}//Draw()

void BallManager::DrawStatic( SpriteBatch *batch, float x, float y ) {
	pImpl->staticBall->Position.x = x;
	pImpl->staticBall->Position.y = y;
	pImpl->staticBall->Draw( batch );
}//DrawStatic()

void BallManager::SetPowerLevel( std::uint8_t power ) {
	pImpl->globalPower	= power;
	pImpl->powerTimer	= 0.0f;

	for ( auto &ball : pImpl->balls ) {
		ball->SetPower( power );
	}
}//SetPowerLevel(power)

void BallManager::SetVisibility( bool visible ) {
	pImpl->globalVisibility = visible;
	pImpl->visibleTimer		= 0.0f;

	for ( auto &ball : pImpl->balls ) {
		ball->SetInvisibility( !visible );
	}
}//SetVisibility()

bool BallManager::IsInvisible() const {
	return !pImpl->globalVisibility;
}

bool BallManager::IsBouncy() const {
	return pImpl->globalPower == 0;
}

bool BallManager::IsPowerful() const {
	return pImpl->globalPower > 1;
}

int BallManager::GetVisibleTime() const {
	return Utility::ftoi( ItemDuration - pImpl->visibleTimer );
}

int BallManager::GetPowerTime() const {
	return Utility::ftoi( ItemDuration - pImpl->powerTimer );
}

bool BallManager::IsWallActive() const {
	return pImpl->wallActive;
}

int BallManager::GetWallTime() const {
	return Utility::ftoi( ItemDuration - pImpl->wallTimer );
}
