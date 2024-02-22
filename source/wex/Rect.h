#pragma once

#include "Point.h"
#include "Size.h"

#include <Windows.h>

namespace wex {

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

class Rect : public RECT {
public:
	Rect() {
	}

	Rect( int l, int t, int r, int b ) {
		left   = l;
		top    = t;
		right  = r;
		bottom = b;
	}

	Rect( RECT r ) 
		: RECT( r )
	{
	}

	Rect( POINT tl, POINT br ) 
		: Rect( tl.x, tl.y, br.x, br.y )
	{
	}

	Rect( POINT tl, SIZE sz ) 
		: Rect( tl.x, tl.y, tl.x + sz.cx, tl.y + sz.cy )
	{
	}

	Rect & operator=( RECT rhs ) {
		left   = rhs.left;
		top    = rhs.top;
		right  = rhs.right;
		bottom = rhs.bottom;

		return *this;
	}

	Rect & operator+=( POINT rhs ) {
		left += rhs.x;
		top += rhs.y;
		right += rhs.x;
		bottom += rhs.y;

		return *this;
	}

	Rect & operator-=( POINT rhs ) {
		left -= rhs.x;
		top -= rhs.y;
		right -= rhs.x;
		bottom -= rhs.y;

		return *this;
	}

	int Width() const {
		return right - left;
	}

	int Height() const {
		return bottom - top;
	}

	Point TopLeft() const {
		return Point( left, top );
	}

	Point BottomRight() const {
		return Point( right, bottom );
	}

	friend Rect operator+( Rect r, POINT p ) {
		return Rect( r.left + p.x, r.top + p.y, r.right + p.x, r.bottom + p.y );
	}

	friend Rect operator-( Rect r, POINT p ) {
		return Rect( r.left - p.x, r.top - p.y, r.right - p.x, r.bottom - p.y );
	}

	Rect Inflated( int value ) const {
		return Rect( left - value, top - value, right + value, bottom + value );
	}

	bool DoOverlap( RECT r ) const {
		if( right < r.left || r.right < left ) {
			return false;
		}

		if( bottom < r.top || r.bottom < top ) {
			return false;
		}

		return true;
	}

	bool DoOverlap( POINT p ) const {
		return ::PtInRect( this, p ) != 0;
	}
};

} // namespace wex
