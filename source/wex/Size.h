#pragma once

#include <Windows.h>

namespace wex {

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

class Size : public SIZE {
public:
	Size() {
	}

	Size( SIZE p ) 
		: SIZE( p )
	{
	}

	Size( int x, int y ) {
		cx = x;
		cy = y;
	}

	Size( LPARAM lParam ) {
		cx = ( (int) (short) LOWORD( lParam ) );
		cy = ( (int) (short) HIWORD( lParam ) );
	}

	Size operator+( SIZE rhs ) const {
		return Size( cx + rhs.cx, cy + rhs.cy );
	}

	Size operator-( SIZE rhs ) const {
		return Size( cx - rhs.cx, cy - rhs.cy );
	}

	bool operator==( SIZE rhs ) const {
		return ( cx == rhs.cx ) && ( cy == rhs.cy );
	}

	bool operator!=( SIZE rhs ) const {
		return !( *this == rhs );
	}
};

} // namespace wex
