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
#include "Highscore.h"

using namespace WinGame;
using namespace WinGame::Game;

/*
==================================
ddHighscore()
Constructor of the class
==================================
*/
ddHighscore::ddHighscore() :
	entries() {

		using namespace Windows::System::UserProfile;
		userName = UserInformation::GetDisplayNameAsync()->GetResults();
}

/*
==================================
~ddHighscore()
Destructor of the class
==================================
*/
ddHighscore::~ddHighscore() {
}

Platform::String^ ddHighscore::GetUserName() const {
	return userName;
}

/*
==================================
GetHighscoreCount()
Gets the overall count of highscore entries
==================================
*/
highscoreSize_t ddHighscore::GetHighscoreCount() const {
	return entries.size();
}

/*
==================================
GetHighscore()
Gets a specific highscore entry at the given index.
==================================
*/
const highscore_s* ddHighscore::GetHighscore( highscoreSize_t index ) const {
	return &entries[index];
}

/*
==================================
AddHighscore()
Adds a new Highscore and returns true if it is actually a new one
==================================
*/
bool ddHighscore::AddHighscore( Platform::String^ name, const std::uint32_t points, const highscoreSize_t limit ) {
	if ( limit == 0 || points == 0 ) {
		return false;
	}
	
	highscore_s hs;
	hs.name = name;
	hs.points = points;
	entries.push_back( hs );

	if ( entries.size() == 1 ) {
		return true;
	}

	std::sort( std::begin( entries ), std::end( entries ), std::greater<highscore_s>() );
	
	if ( entries.size() > limit ) {
		entries.resize( limit );
	}

	auto result = std::find_if( std::begin( entries ), std::end( entries ), [&]( const highscore_s &obj ) {
		return hs.name == obj.name && hs.points == obj.points;
	} );
	return result != std::end( entries );
}

/*
==================================
SaveHighscore()
Saves all the highscore entries into the App Settings File
==================================
*/
void ddHighscore::SaveHighscore() const {
	auto settings	= ApplicationData::Current->LocalSettings;
	auto container	= settings->CreateContainer( "WinGameHighscore", ApplicationDataCreateDisposition::Always );

	if ( settings->Containers->HasKey( "WinGameHighscore" ) ) {
		auto values = settings->Containers->Lookup( "WinGameHighscore" )->Values;

		values->Clear();
		values->Insert( "count", entries.size() );
		
		highscoreSize_t		key = 0;
		for ( const auto &entry : entries ) {	

			std::wstringstream strName;
			std::wstringstream strPoints;

			strName		<< "name"	<< key;
			strPoints	<< "points"	<< key;
			
			values->Insert( ref new Platform::String( strName.str().c_str() ), entry.name );
			values->Insert( ref new Platform::String( strPoints.str().c_str() ), entry.points );

			++key;
		}
	}
}

/*
==================================
ReadHighscore()
Reads all the highscore entries from the App Settings File
==================================
*/
void ddHighscore::ReadHighscore() {
	auto settings		= ApplicationData::Current->LocalSettings;
	bool hasContainer	= settings->Containers->HasKey( "WinGameHighscore" );

	entries.clear();
	
	if ( hasContainer ) {
		auto values		= settings->Containers->Lookup( "WinGameHighscore" )->Values;
		bool hasSetting = values->HasKey( "count" );
	
		if ( hasSetting ) {
			highscoreSize_t count = safe_cast<highscoreSize_t>( values->Lookup( "count" ) );
			
			for ( decltype( count ) i = 0; i < count; ++i ) {
				std::wstringstream strName;
				std::wstringstream strPoints;

				strName		<< "name"	<< i;
				strPoints	<< "points"	<< i;

				highscore_s highscore;
				auto		name	= ref new Platform::String( strName.str().c_str() );
				auto		points	= ref new Platform::String( strPoints.str().c_str() );

				if ( values->HasKey( name ) ) {
					highscore.name = safe_cast<Platform::String^>( values->Lookup( name ) );
					values->Remove( name );
				}

				if ( values->HasKey( points ) ) {
					highscore.points = safe_cast<std::uint32_t>( values->Lookup( points ) );
					values->Remove( points );
				}

				entries.push_back( std::move( highscore ) );
			}
		}

		values->Remove( "count" );
		settings->DeleteContainer( "WinGameHighscore" );
	}

	std::sort( std::begin( entries ), std::end( entries ), std::greater<highscore_s>() );
}
