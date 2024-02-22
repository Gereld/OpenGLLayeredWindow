#pragma once

#include "Rect.h"

#include <memory>
#include <Windows.h>

namespace wex {

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

class Window : public std::enable_shared_from_this<Window> {
public:
	HWND m_hWnd = nullptr;

public:
	Window();
	virtual ~Window();

	bool CreateEx( DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, const wex::Rect & rect, HWND hWndParent, HMENU hMenu );
	void Destroy();

	virtual LRESULT WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	operator HWND() { return m_hWnd; }
	operator HWND() const { return m_hWnd; }

	static LRESULT CALLBACK ClassWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

} // namespace wex

