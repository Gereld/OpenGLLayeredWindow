#include "Bitmap.h"

#include <stdexcept>

namespace wex {

Bitmap::Bitmap() {
	m_size.cx = 0;
	m_size.cy = 0;
}

Bitmap::Bitmap( HINSTANCE hInstance, LPCWSTR lpszResourceName ) {
	m_size.cx = 0;
	m_size.cy = 0;
	LoadFromResource( hInstance, lpszResourceName );
}

Bitmap::Bitmap( HINSTANCE hInstance, unsigned int nResourceID ) {
	m_size.cx = 0;
	m_size.cy = 0;
	LoadFromResource( hInstance, MAKEINTRESOURCEW( nResourceID ) );
}

Bitmap::Bitmap( int width, int height ) {
	m_size.cx = width;
	m_size.cy = height;
}

Bitmap::~Bitmap() {
	Destroy();
}

void Bitmap::LoadFromResource( HINSTANCE hInstance, LPCWSTR lpszResourceName ) {
	m_hBitmap = ::LoadBitmapW( hInstance, lpszResourceName );

	if( m_hBitmap == nullptr ) {
		throw std::runtime_error( "Bitmap::LoadFromResource" );
	}

	BITMAP bitmap;
	::GetObject( m_hBitmap, sizeof( bitmap ), &bitmap );

	m_size.cx = bitmap.bmWidth;
	m_size.cy = bitmap.bmHeight;
}

void Bitmap::Destroy() {
	::DeleteObject( m_hBitmap );
	m_hBitmap = nullptr;
}

} // namespace wex
