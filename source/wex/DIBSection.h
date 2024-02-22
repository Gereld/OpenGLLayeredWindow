#pragma once

#include "Bitmap.h"

#include <Windows.h>

namespace wex {

enum class CompressionFormat : DWORD {
	RGB       = BI_RGB,
	RLE8      = BI_RLE8,
	RLE4      = BI_RLE4,
	BITFILEDS = BI_BITFIELDS,
	JPEG      = BI_JPEG,
	PNG       = BI_PNG,
};

class DIBSection : public Bitmap {
protected:
	BITMAPINFOHEADER m_infoheader;
	unsigned int *   m_bits = nullptr;

	void Init( int width, int height, unsigned int pixel_depth, CompressionFormat format );

public:
	DIBSection();
	DIBSection( int width, int height, unsigned int pixel_depth = 32, CompressionFormat format = CompressionFormat::RGB );
	DIBSection( const Bitmap & source, unsigned int pixel_depth = 32, CompressionFormat format = CompressionFormat::RGB );
	~DIBSection() override;

	virtual void Create( int width, int height, unsigned int pixel_depth = 32, CompressionFormat format = CompressionFormat::RGB );

public:
	int                  Size() const { return m_infoheader.biSizeImage; }
	unsigned int *       Bits() { return m_bits; }
	const unsigned int * Bits() const { return m_bits; }
};

} // namespace wex
