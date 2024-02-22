#pragma once

#include <Windows.h>

namespace wex {

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

class WindowClass : public WNDCLASSEXW {
	ATOM m_atom = 0;

public:
	WindowClass();
	WindowClass( WindowClass && rhs ) noexcept;
	~WindowClass();

	WindowClass & operator=( WindowClass && rhs ) noexcept;

	bool Register();
	void Unregister();

	const ATOM GetAtom() const { return m_atom; }
};

//*************************************************************************************************

WindowClass register_default_frame_window( LPCWSTR class_name, HINSTANCE hInstance );

} // namespace wex

