#pragma once

#include <Windows.h>

namespace wex {

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

class Point : public POINT {
public:
	Point() {
	}

	Point( POINT p ) {
		x = p.x;
		y = p.y;
	}

	Point( int x, int y ) {
		this->x = x;
		this->y = y;
	}

	Point( LPARAM lParam ) {
		x = ( (int) (short) LOWORD( lParam ) );
		y = ( (int) (short) HIWORD( lParam ) );
	}

	Point operator+( POINT rhs ) const {
		return Point( x + rhs.x, y + rhs.y );
	}

	Point operator-( POINT rhs ) const {
		return Point( x - rhs.x, y - rhs.y );
	}

	LONG ToLong() const {
		return MAKELONG( x, y );
	}

	bool operator==( const POINT & rhs ) const {
		return ( x == rhs.x ) && ( y == rhs.y );
	}

	bool operator!=( const POINT & rhs ) const {
		return !( *this == rhs );
	}
};

} // namespace wex
