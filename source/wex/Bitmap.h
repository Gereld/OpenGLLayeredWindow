#pragma once

#include <Windows.h>

namespace wex {

class Bitmap {
protected:
	HBITMAP m_hBitmap = nullptr;
	SIZE    m_size;

	void LoadFromResource( HINSTANCE hInstance, LPCWSTR lpszResourceName );

public:
	Bitmap();
	Bitmap( HINSTANCE hInstance, LPCWSTR lpszResourceName );
	Bitmap( HINSTANCE hInstance, unsigned int nResourceID );
	Bitmap( int width, int height );
	virtual ~Bitmap();

	virtual void Destroy();

public:
	int Width() const { return m_size.cx; }
	int Height() const { return m_size.cy; }

	HBITMAP Handle() const { return m_hBitmap; }
};

} // namespace wex
