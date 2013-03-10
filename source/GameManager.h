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

#include "GraphicsManager.h"
#include "InputManager.h"
#include "AudioManager.h"
#include "ContentManager.h"
#include "VirtualResolution.h"
#include "IStyle.h"
#include "BasicStyle.h"
#include "Highscore.h"

namespace BreakIt {
	namespace Objects {
		class GameplayManager;
	}
}

namespace WinGame {
	namespace Game {
		class GameManager;

		class GameState abstract {
		public:
			virtual void Load( GameManager *manager ) {
				gameManager = manager;
			}//Load(manager)

			virtual void Unload()	= 0;
			virtual void Pause()	= 0;
			virtual void Resume()	= 0;

			virtual void Update( float elapsedTime, float totalTime )	= 0;
			virtual void Draw( float elapsedTime, float totalTime )		= 0;

			template<typename T>
			static std::unique_ptr<GameState> Create() {
				static_assert( std::is_base_of<GameState, T>::value, "T has to be derived from type [GameState]." );
				return std::make_unique<T>();
			}//Create()

			template<typename T, typename Arg0>
			static std::unique_ptr<GameState> Create( Arg0&& arg0 ) {
				static_assert( std::is_base_of<GameState, T>::value, "T has to be derived from type [GameState]." );
				return std::make_unique<T>( arg0 );
			}//make_unique()

			template<typename T, typename Arg0, typename Arg1>
			static std::unique_ptr<GameState> Create( Arg0&& arg0, Arg1&& arg1 ) {
				static_assert( std::is_base_of<GameState, T>::value, "T has to be derived from type [GameState]." );
				return std::make_unique<T>( arg0, arg1 );
			}//make_unique()

			template<typename T, typename Arg0, typename Arg1, typename Arg2>
			static std::unique_ptr<GameState> Create( Arg0&& arg0, Arg1&& arg1, Arg2&& arg2 ) {
				static_assert( std::is_base_of<GameState, T>::value, "T has to be derived from type [GameState]." );
				return std::make_unique<T>( arg0, arg1, arg2 );
			}//make_unique()

		protected:
			GameManager *gameManager;
		};//GameScreen class

		class GameManager final {
		public:
			explicit GameManager();
			virtual ~GameManager();
			UTILITY_CLASS_MOVE( GameManager );

			bool			isResumed;
			int				levelIndex;
			std::uint32_t	tempPoints;
			std::uint8_t	tempHealth;

			Graphics::GraphicsManager*			GetGraphicsManager() const;
			Input::InputManager*				GetInputManager() const;
			Content::ContentManager*			GetMenuContent() const;
			Content::ContentManager*			GetGameplayContent() const;
			Audio::AudioManager*				GetAudioManager() const;
			DirectX::SpriteBatch*				GetSpriteBatch() const;
			Graphics::VirtualResolution*		GetVirtualResolution() const;
			BreakIt::Styles::IStyle*			GetStyleManager() const;
			BreakIt::Objects::GameplayManager*	GetGameplayManager() const;
			
			ddHighscore*		GetHighscore();
			const ddHighscore*	GetHighscore() const;

			int GetFPS() const;

			void Initialize( Windows::UI::Core::CoreWindow ^gameWindow, float width = 1024.0f, float height = 768.0f );
			bool Run( bool isWindowVisible );
			void Suspend();
			void Resume();
			void Resize();

			void ResumeGame( std::unique_ptr<BreakIt::Styles::IStyle> &&style, std::unique_ptr<GameState> &&state );
			void PushGameState( std::unique_ptr<GameState> &&gameState );
			void PopGameState();
			void ChangeGameState( std::unique_ptr<GameState> &&gameState );

			void SetStyleManager( std::unique_ptr<BreakIt::Styles::IStyle> &&styleManager );

		private:
			UTILITY_CLASS_COPY( GameManager );
			UTILITY_CLASS_PIMPL();

		};//GameManager class

	}//Game namespace
}//BreakIt namespace
