#include "WindowClass.h"

#include "Window.h"

namespace wex {

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

WindowClass::WindowClass() {
	memset( this, 0, sizeof( WNDCLASSEXW ) );
	cbSize = sizeof( WNDCLASSEXW );
}

WindowClass::WindowClass( WindowClass && rhs ) noexcept {
	memcpy( this, &rhs, sizeof( WNDCLASSEXW ) );
	m_atom     = rhs.m_atom;
	rhs.m_atom = 0;
}

WindowClass & WindowClass::operator=( WindowClass && rhs ) noexcept {
	static_cast<WNDCLASSEXW>( *this ) = rhs;
	using std::swap;
	swap( m_atom, rhs.m_atom );
	return *this;
}

bool WindowClass::Register() {
	m_atom = ::RegisterClassExW( this );
	return m_atom != 0;
}

void WindowClass::Unregister() {
	if( m_atom ) {
		UnregisterClassW( lpszClassName, hInstance );
		m_atom = 0;
	}
}

WindowClass::~WindowClass() {
	Unregister();
}

//*************************************************************************************************

WindowClass register_default_frame_window( LPCWSTR class_name, HINSTANCE hInstance ) {
	WindowClass wc;
	wc.lpszClassName = class_name;
	wc.style         = CS_BYTEALIGNCLIENT | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_GLOBALCLASS;
	wc.lpfnWndProc   = wex::Window::ClassWindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof( wex::Window * ); // size of GWL_USERDATA
	wc.hInstance     = hInstance;
	wc.hIcon         = 0;
	wc.hCursor       = LoadCursorW( nullptr, IDC_ARROW );
	wc.hbrBackground = (HBRUSH) ( COLOR_GRADIENTACTIVECAPTION );
	wc.lpszMenuName  = nullptr;
	wc.hIconSm       = 0;

	wc.Register();

	return wc;
}

} // namespace wex
