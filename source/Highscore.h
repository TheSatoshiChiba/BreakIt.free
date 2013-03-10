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
	namespace Game {
	
		struct highscore_s {
			Platform::String	^name;
			std::uint32_t		points;
		};

		typedef std::vector<highscore_s>	highscores_t;
		typedef highscores_t::size_type		highscoreSize_t;
		
		inline bool operator==( const highscore_s &left, const highscore_s &right ) {
			return left.points == right.points;
		}

		inline bool operator!=( const highscore_s &left, const highscore_s &right ) {
			return left.points != right.points;
		}

		inline bool operator<( const highscore_s &left, const highscore_s &right ) {
			return left.points < right.points;
		}

		inline bool operator>( const highscore_s &left, const highscore_s &right ) {
			return left.points > right.points;
		}

		inline bool operator<=( const highscore_s &left, const highscore_s &right ) {
			return left.points <= right.points;
		}

		inline bool operator>=( const highscore_s &left, const highscore_s &right ) {
			return left.points >= right.points;
		}

		/*
		===============================================
		This class helps to store and read a 3 player
		local Highscore file.
		===============================================
		*/
		class ddHighscore sealed {
		public:
			ddHighscore();
			virtual ~ddHighscore();

			highscoreSize_t		GetHighscoreCount() const;
			const highscore_s*	GetHighscore( const highscoreSize_t index ) const;
			bool				AddHighscore( Platform::String^ name, const std::uint32_t points, const highscoreSize_t limit );
			void				ReadHighscore();
			void				SaveHighscore() const;
			Platform::String^	GetUserName() const;

		private:
			highscores_t entries;
			Platform::String ^userName;
		};
	}
}
