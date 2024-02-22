#include "DIBSection.h"

#include "DC.h"

#include <Windows.h>

namespace wex {

DIBSection::DIBSection() {
}

DIBSection::DIBSection( int width, int height, unsigned int pixel_depth, CompressionFormat format ) {
	Init( width, height, pixel_depth, format );
}

DIBSection::DIBSection( const Bitmap & source, unsigned int pixel_depth, CompressionFormat format ) {
	Init( source.Width(), source.Height(), pixel_depth, format );

	BITMAPINFO bi = { 0 };
	// +todo+ ::ZeroMemory( &bi, sizeof( bi ) );
	bi.bmiHeader.biSize        = sizeof( BITMAPINFOHEADER );
	bi.bmiHeader.biWidth       = source.Width();
	bi.bmiHeader.biHeight      = -source.Height();
	bi.bmiHeader.biPlanes      = 1;
	bi.bmiHeader.biBitCount    = static_cast<WORD>( pixel_depth );
	bi.bmiHeader.biCompression = static_cast<DWORD>( format );

	CompatibleDC dc;
	::GetDIBits( dc, source.Handle(), 0, source.Height(), m_bits, &bi, DIB_RGB_COLORS );
}

DIBSection::~DIBSection() {
}

void DIBSection::Init( int width, int height, unsigned int pixel_depth, CompressionFormat format ) {
	// +todo+ ::ZeroMemory( &m_infoheader, sizeof( m_infoheader ) );
	m_infoheader = { 0 };

	m_infoheader.biSize        = sizeof( m_infoheader );
	m_infoheader.biWidth       = width;
	m_infoheader.biHeight      = -height; // by default a DIBSECTION is bottom-up, a negative height value makes it top-down
	m_infoheader.biPlanes      = 1;
	m_infoheader.biBitCount    = static_cast<WORD>( pixel_depth );
	m_infoheader.biCompression = static_cast<DWORD>( format );

	int scanline             = ( ( width * ( pixel_depth / 8 ) + 1 ) / 2 ) * 2;
	m_infoheader.biSizeImage = scanline * abs( height );

	m_hBitmap = ::CreateDIBSection( nullptr, (BITMAPINFO *) &m_infoheader, DIB_RGB_COLORS, (void **) &m_bits, nullptr, 0 );

	m_size.cx = width;
	m_size.cy = abs( height );
}

void DIBSection::Create( int width, int height, unsigned int pixel_depth, CompressionFormat format ) {
	Destroy();
	Init( width, height, pixel_depth, format );
}

} // namespace wex
