#pragma once

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

namespace pix {

// A view over an image. Like std::string_view but for images. 
// Allows access to a sub-rectangle of an image.
class View {
protected:
	unsigned int * m_bits   = nullptr;
	int            m_width  = 0;
	int            m_height = 0;
	int            m_stride = 0;

public:
	View() = default;
	View( const View & ) = default;
	View( View && ) = default;
	~View() = default;
	View & operator=( const View & rhs ) = default;
	View & operator=( View && rhs ) = default;

	View( unsigned int * bits, int width, int height, int stride ) 
		: m_bits{ bits }
		, m_width{ width }
		, m_height{ height }
		, m_stride{ stride }
	{
	}

public:
	unsigned int * Bits() { return m_bits; }
	const unsigned int * Bits() const { return m_bits; }

	int Width() const { return m_width; }
	int Height() const { return m_height; }
	int Stride() const { return m_stride; }

	unsigned int * operator[]( int i ) { return (unsigned int *) ( (unsigned char *) m_bits + m_stride * i ); }
	const unsigned int * operator[]( int i ) const { return (const unsigned int *) ( (const unsigned char *) m_bits + m_stride * i ); }
};

// Returns a view which is a sub-region of another view or image.
inline View view( View src, int x, int y, int w, int h ) {
	return View( src.Bits() + y * src.Stride() + x, w, h, src.Stride() );
}

template<class OP>
void transform( View dst, OP op ) {
	int width  = dst.Width();
	int height = dst.Height();

	for( int i = 0; i < height; i++ ) {
		unsigned int * dstl = dst[i];

		for( int k = 0; k < width; k++ ) {
			dstl[k] = op( dstl[k] );
		}
	}
}

constexpr unsigned int alpha( unsigned int pix ) {
	return pix >> 24;
}

constexpr unsigned int multiply_pixel_alpha( unsigned int pix ) {
	unsigned int a = alpha( pix );

	unsigned int pe = pix & 0x00FF00FF;
	unsigned int e = ( pe * a ) >> 8;

	unsigned int po = ( pix >> 8 ) & 0x00FF00FF;
	unsigned int o = po * a;

	return ( e & 0x00FF00FF ) | ( o & 0x0000FF00 ) | ( pix & 0xFF000000 );
}
		
void multiply_alpha( View dst );

} // namespace pix
