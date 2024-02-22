#pragma once 

#include "Rect.h"
#include "Size.h"

#include <Windows.h>

namespace wex {

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

class DC {
protected:
	HDC m_hDC = nullptr;

public:
	operator HDC() {
		return m_hDC;
	}
};

class AnyDC : public DC {
public:
	AnyDC() {
	}

	AnyDC( HDC hDC )  {
		 m_hDC = hDC;
	}

	~AnyDC() {
		::DeleteDC( m_hDC );
	}
};

class WndDC : public DC {
protected:
	HWND m_hWnd;

public:
	~WndDC() {
		::ReleaseDC( m_hWnd, m_hDC );
	}
};

class DCEX : public WndDC {
	// Feng Yuan's Windows Graphics book calls this the "undocumented flag that makes GetDCEx() succeed" flag.
	static constexpr DWORD DCX_USESTYLE = 0x10000; 

public:
	DCEX( HWND hWnd ) {
		m_hWnd = hWnd;
		m_hDC  = ::GetDCEx( m_hWnd, (HRGN) 1, DCX_WINDOW | DCX_INTERSECTRGN | DCX_CLIPCHILDREN | DCX_CLIPSIBLINGS | DCX_VALIDATE | DCX_USESTYLE );
	}

	DCEX( HWND hWnd, HRGN hRGN ) {
		m_hWnd = hWnd;
		m_hDC  = ::GetDCEx( m_hWnd, hRGN, DCX_WINDOW | DCX_INTERSECTRGN | DCX_CLIPCHILDREN | DCX_CLIPSIBLINGS | DCX_VALIDATE | DCX_USESTYLE );
	}

	DCEX( HWND hWnd, HRGN hRGN, DWORD flags ) {
		m_hWnd = hWnd;
		m_hDC  = ::GetDCEx( m_hWnd, hRGN, flags | DCX_USESTYLE );
	}

	~DCEX() = default;
};

class WindowDC : public WndDC {
public:
	WindowDC( HWND hWnd ) {
		m_hWnd = hWnd;
		m_hDC  = ::GetWindowDC( hWnd );
	}

	~WindowDC() = default;
};

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

class CompatibleDC : public AnyDC {
public:
	CompatibleDC() {
		m_hDC = ::CreateCompatibleDC( nullptr );
	}

	CompatibleDC( HDC hDC ) {
		m_hDC = ::CreateCompatibleDC( hDC );
	}

	~CompatibleDC() = default;
};

class PaintDC : public DC {
public:
	PAINTSTRUCT m_ps;
	HWND        m_hWnd;

	PaintDC( HWND hWnd ) {
		m_hWnd = hWnd;
		m_hDC  = ::BeginPaint( m_hWnd, &m_ps );
	}

	~PaintDC() {
		::EndPaint( m_hWnd, &m_ps );
	}
};

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

class MemDC : public DC {
private:
	HBITMAP   m_hBitmap;    // Offscreen bitmap
	HBITMAP   m_hOldBitmap; // bitmap originally found in MemDC
	HDC       m_hDestDC;    // Saves DC passed in constructor
	wex::Rect m_rect;       // Rectangle of drawing area.

public:
	MemDC( HDC hDestDC, const RECT * pRect ) {
		m_hDestDC = hDestDC;

		// Get the rectangle to draw
		if( pRect == nullptr ) {
			::GetClipBox( m_hDestDC, &m_rect );
		}
		else {
			m_rect = *pRect;
		}

		// Create a Memory DC
		m_hDC = ::CreateCompatibleDC( m_hDestDC );
		::LPtoDP( m_hDestDC, (LPPOINT) &m_rect, 2 );

		m_hBitmap    = ::CreateCompatibleBitmap( m_hDestDC, m_rect.Width(), m_rect.Height() );
		m_hOldBitmap = ( HBITMAP )::SelectObject( m_hDC, m_hBitmap );

		::SetMapMode( m_hDC, ::GetMapMode( m_hDestDC ) );

		wex::Size size;
		::GetWindowExtEx( m_hDestDC, &size );
		::SetWindowExtEx( m_hDC, size.cx, size.cy, nullptr );

		::GetViewportExtEx( m_hDestDC, &size );
		::SetViewportExtEx( m_hDC, size.cx, size.cy, nullptr );

		::DPtoLP( m_hDestDC, (LPPOINT) &m_rect, 2 );
		::SetWindowOrgEx( m_hDC, m_rect.left, m_rect.top, nullptr );
	}

	~MemDC() {
		// Copy the offscreen bitmap onto the screen.
		::BitBlt( m_hDestDC, m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), m_hDC, m_rect.left, m_rect.top, SRCCOPY );

		//Swap back the original bitmap.
		::SelectObject( m_hDC, m_hOldBitmap );
		::DeleteObject( m_hBitmap );

		::DeleteDC( m_hDC );
	}
};

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

template<class HH, unsigned int ID>
class SaveResource {
	HDC m_hDC;
	HH  m_hOldResource;

public:
	SaveResource( HDC hDC ) {
		m_hDC          = hDC;
		m_hOldResource = ( HH )::GetCurrentObject( m_hDC, ID );
	}

	SaveResource( HDC hDC, HH hNewResource ) {
		m_hDC          = hDC;
		m_hOldResource = ( HH )::SelectObject( m_hDC, hNewResource );
	}

	~SaveResource() {
		::SelectObject( m_hDC, m_hOldResource );
	}
};

using SaveBitmap = SaveResource<HBITMAP, OBJ_BITMAP>;
using SaveFont   = SaveResource<HFONT, OBJ_FONT>;
using SaveBrush  = SaveResource<HBRUSH, OBJ_BRUSH>;
using SavePen    = SaveResource<HPEN, OBJ_PEN>;
using SaveRegion = SaveResource<HRGN, OBJ_REGION>;

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

template<class TT>
class GDIObject {
public:
	TT m_handle;

public:
	GDIObject( TT handle ) {
		m_handle = handle;
	}

	~GDIObject() {
		::DeleteObject( m_handle );
	}

	operator const TT() const {
		return m_handle;
	}

	operator TT() {
		return m_handle;
	}
};

using HFont   = GDIObject<HFONT>;
using HPen    = GDIObject<HPEN>;
using HBrush  = GDIObject<HBRUSH>;
using HBitmap = GDIObject<HBITMAP>;

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

inline void FillSolidRect( HDC hDC, LPCRECT lpRect, COLORREF clr ) {
	::SetBkColor( hDC, clr );
	::ExtTextOut( hDC, 0, 0, ETO_OPAQUE, lpRect, nullptr, 0, nullptr );
}

inline void FillSolidRect( HDC hDC, int x, int y, int cx, int cy, COLORREF clr ) {
	RECT rect = { x, y, x + cx, y + cy };
	FillSolidRect( hDC, &rect, clr );
}

} // namespace wex
