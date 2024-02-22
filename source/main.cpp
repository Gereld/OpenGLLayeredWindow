#include "gl.h"
#include "View.h"
#include "wex/DC.h"
#include "wex/WindowClass.h"
#include "wex/Window.h"
#include "wex/DIBSection.h"
#include "wgl/OpenGLContext.h"
#include "pix/Pixmap.h"
#include "io/Wavefront.h"
#include "io/ReadFile.h"
#include "wex/Resource.h"

#include <ShellApi.h>
#include <ShellScalingAPI.h>
#pragma comment( lib, "Shcore.lib" )

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

class MyWindow : public wex::Window {
public:
	wgl::OpenGLContext m_context;
	View               m_view;
	wex::DIBSection    m_dib;

public:
	MyWindow() 
		: m_view( m_dib )
	{
	}

	LRESULT WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) override {
		switch (uMsg) {
		case WM_CREATE:
			return OnCreate( reinterpret_cast<LPCREATESTRUCT>( lParam ) );

		case WM_SIZE:
			OnSize( wParam, wex::Size( lParam ) );
			return TRUE;

		case WM_NCHITTEST:
			return OnNcHitTest( wex::Point( lParam ) );

		case WM_KEYDOWN:
			if( wParam == VK_ESCAPE ) {
				OnClose();
			}
			return TRUE;

		case WM_CLOSE:
			OnClose();
			return TRUE;
		}
		return DefWindowProcW( hWnd, uMsg, wParam, lParam );
	}

	LRESULT OnCreate( LPCREATESTRUCT lpCreateStruct ) {
		HDC hDC = GetDC( m_hWnd );
		
		// Create an OpengL 2.x context
		m_context.Create( hDC );
		m_context.MakeCurrent();

		// Load WGL Extensions. wglCreateContextAttribsARB now becomes available.
		gladLoadWGL( hDC );

		// Create an OpenGL 3.3 context based on the given attributes
		int attributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3, // Set the MAJOR version of OpenGL to 3
			WGL_CONTEXT_MINOR_VERSION_ARB, 3, // Set the MINOR version of OpenGL to 3
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, // Use core profile
			0
		};
		HGLRC hRC = wglCreateContextAttribsARB( hDC, NULL, attributes );
		if( hRC == nullptr ) {
			return FALSE;
		}

		// Assign the newly created context for wex::OpenGLContext to manage
		m_context.Destroy();
		m_context.m_hGLRC = hRC;
		m_context.MakeCurrent();

		// Load GL functions
		if( !gladLoadGL() ) {
			return FALSE;
		}

		// Enable vsync
		wglSwapIntervalEXT( 1 );

		wex::Rect wr;
		GetWindowRect( m_hWnd, &wr );

		// Create a DIBSECTION to the dimensions of the window
		// The height is negated because we want a bottom-up DIBSECTION (like OpenGL buffers/textures)
		m_dib.Create( wr.Width(), -wr.Height(), 32, wex::CompressionFormat::RGB );

		// Create the view. The view will do the drawing into the DIBSECTION. 
		m_view.OnCreate( wr.Width(), wr.Height() );

		return TRUE;
	}

	void OnClose() {
		m_view.OnDestroy();
		m_context.Destroy();

		DestroyWindow( m_hWnd );
		::PostQuitMessage( 0 );
	}

	void OnSize( WPARAM wParam, wex::Size size ) {
		m_view.OnSize( size.cx, size.cy );
	}

	LRESULT OnNcHitTest( wex::Point point )  {
		// Treat all clicks as if we click on the caption. It's an easy way to move the window.
		return HTCAPTION;
	}

	void OpenFile( const std::filesystem::path & filename ) {
		std::vector<glm::vec3> vertices;
		std::vector<std::array<int, 3>> triangles;
		std::string buffer = io::read_text_file( filename );
		io::basic_wavefront_obj_importer( buffer, vertices, triangles );
		m_view.SetMesh( std::move( vertices ), std::move( triangles ) );
	}

	void Render() {
		// Draw the scene into the image
		m_view.Render();
		m_context.SwapBuffers();

		// Update the window content with UpdateLayeredWindow
		{
			// UpdateLayeredWindow required pre-multiplied alpha
			pix::View v{ m_dib.Bits(), m_dib.Width(), m_dib.Height(), static_cast<int>( m_dib.Width() * sizeof( unsigned int ) ) };
			pix::multiply_alpha( v );

			wex::DIBSection & bmp   = m_dib;
			wex::AnyDC        wndDC = GetDC( NULL );
			wex::CompatibleDC bmpDC( wndDC );
			HBITMAP           hOldBitmap = (HBITMAP) SelectObject( bmpDC, bmp.Handle() );

			POINT dst = { 0, 0 };
			ClientToScreen( m_hWnd, &dst );

			POINT         src  = { 0, 0 };
			SIZE          size = { bmp.Width(), bmp.Height() };
			BLENDFUNCTION bf   = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
			UpdateLayeredWindow( m_hWnd, wndDC, &dst, &size, bmpDC, &src, 0, &bf, ULW_ALPHA );

			SelectObject( bmpDC, hOldBitmap );
		}
	}
};

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow 
) {
	// Enable DPI awareness (ie disable system-wide High-DPI scaling)
	SetProcessDpiAwareness( PROCESS_PER_MONITOR_DPI_AWARE );

	// Register our window class
	wex::WindowClass wc = wex::register_default_frame_window( L"LayeredWindow", hInstance );

	wex::Size screen{ GetSystemMetrics( SM_CXSCREEN ), GetSystemMetrics( SM_CYSCREEN ) };
	wex::Rect rc{ 0, 0, screen.cx / 2, screen.cy / 2 };
	rc += wex::Point{ ( screen.cx - rc.Width() ) / 2, ( screen.cy - rc.Height() ) / 2 };

	// Create the window. The WS_EX_LAYERED style indicate that its content will be updated with UpdateLayeredWindow.
	MyWindow wnd;
	wnd.CreateEx( WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, wc.lpszClassName, L"Hello", WS_VISIBLE | WS_POPUP, rc, NULL, NULL );

	// Load the mesh from the resources
	{
		std::span buffer = wex::get_resource_data( hInstance, L"Mesh" );

		std::vector<glm::vec3> vertices;
		std::vector<std::array<int, 3>> triangles;
		io::basic_wavefront_obj_importer( std::string_view( buffer.data(), buffer.size() ), vertices, triangles );

		// Give the mesh data to the view
		wnd.m_view.SetMesh( std::move( vertices ), std::move( triangles ) );
	}

	MSG  msg;
	bool bContinue = true;

	while( bContinue ) {
		while( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) ) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			if( msg.message == WM_QUIT ) {
				bContinue = false;
			}
		}
		if( !bContinue ) {
			break;
		}

		wnd.Render();
	}

	return (int) msg.wParam;
}
