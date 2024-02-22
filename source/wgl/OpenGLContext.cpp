#include "OpenGLContext.h"

#include <GL/gl.h>

namespace wgl {

OpenGLContext::OpenGLContext() {
	m_hDC   = nullptr;
	m_hGLRC = nullptr;
}

OpenGLContext::~OpenGLContext() {
	Destroy();
}

void OpenGLContext::Create( HDC hDC ) {
	m_hDC = hDC;
	SetupPixelFormat();
	m_hGLRC = ::wglCreateContext( m_hDC );
	if( m_hGLRC ) {
		::wglMakeCurrent( m_hDC, m_hGLRC );
	}
}

void OpenGLContext::Destroy() {
	MakeCurrent();

	if( m_hGLRC ) {
		::wglMakeCurrent( nullptr, nullptr );
		::wglDeleteContext( m_hGLRC );
		m_hGLRC = nullptr;
	}
}

void OpenGLContext::SetupPixelFormat() {
	DWORD dwFlags =
		PFD_SUPPORT_OPENGL | // support OpenGL
		PFD_DRAW_TO_WINDOW | // support window
		PFD_DOUBLEBUFFER |   // double buffering
		PFD_GENERIC_ACCELERATED;

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof( PIXELFORMATDESCRIPTOR ), // size of this pfd
		1,                               // version number
		dwFlags,                         // flags
		PFD_TYPE_RGBA,                   // RGBA type
		24,                              // 24-bit color depth
		0, 0, 0, 0, 0, 0,                // color bits ignored
		0,                               // no alpha buffer
		0,                               // shift bit ignored
		0,                               // no accumulation buffer
		0, 0, 0, 0,                      // accum bits ignored
		32,                              // 32-bit z-buffer
		0,                               // stencil buffer
		0,                               // no auxiliary buffer
		PFD_MAIN_PLANE,                  // main layer
		0,                               // reserved
		0, 0, 0                          // layer masks ignored
	};
	int iPixelFormat = ::ChoosePixelFormat( m_hDC, &pfd );

	::SetPixelFormat( m_hDC, iPixelFormat, &pfd );
}

void OpenGLContext::MakeCurrent() {
	if( m_hGLRC ) {
		::wglMakeCurrent( m_hDC, m_hGLRC );
	}
}

void OpenGLContext::SwapBuffers() {
	::SwapBuffers( m_hDC );
}

bool OpenGLContext::IsCurrent() const {
	return ::wglGetCurrentContext() == m_hGLRC;
}

} // namespace wgl
