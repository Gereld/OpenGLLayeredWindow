#include "Window.h"

#include "WindowClass.h"

namespace wex {

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

Window::Window() {
}

Window::~Window() {
}

LRESULT CALLBACK Window::ClassWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	// If the message is WM_NCCREATE, associate Window pointer with this window
	// Note: WM_NCCREATE is not the first message that this WndProc will process
	if( uMsg == WM_NCCREATE ) {
		LPCREATESTRUCT cs = reinterpret_cast<LPCREATESTRUCT>( lParam );
		::SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( cs->lpCreateParams ) );
		Window * pWnd = reinterpret_cast<Window *>( cs->lpCreateParams );
		pWnd->m_hWnd  = hWnd;
	}

	Window * pWnd = reinterpret_cast<Window *>( ::GetWindowLongPtr( hWnd, GWLP_USERDATA ) );

	LRESULT nResult;
	if( pWnd ) {
		nResult = pWnd->WndProc( hWnd, uMsg, wParam, lParam );
	}
	else {
		nResult = ::DefWindowProc( hWnd, uMsg, wParam, lParam );
	}

	if( uMsg == WM_NCDESTROY ) {
		if( pWnd ) {
			pWnd->m_hWnd = NULL;
		}
	}

	return nResult;
}

bool Window::CreateEx( DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, const wex::Rect & rect, HWND hWndParent, HMENU hMenu ) {
	int       x         = rect.left;
	int       y         = rect.top;
	int       width     = rect.Width();
	int       height    = rect.Height();
	HINSTANCE hInstance = nullptr;
	void *    pParam    = this;
	m_hWnd              = ::CreateWindowExW( dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, width, height, hWndParent, hMenu, hInstance, pParam );

	return m_hWnd != nullptr;
}

void Window::Destroy() {
	if( m_hWnd ) {
		::DestroyWindow( m_hWnd );
	}
}

LRESULT Window::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	return ::DefWindowProcW( hWnd, uMsg, wParam, lParam );
}

} // namespace wex
