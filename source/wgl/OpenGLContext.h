#pragma once

#include "../ogl/OpenGLContext.h"

#include <Windows.h>

namespace wgl {

class OpenGLContext : public ogl::OpenGLContext {
public:
	HDC   m_hDC;
	HGLRC m_hGLRC;

public:
	OpenGLContext();
	~OpenGLContext() override;

	void Create( HDC hDC );
	void Destroy();

	bool IsCurrent() const;

	void MakeCurrent() override;
	void SwapBuffers() override;

protected:
	void SetupPixelFormat();
};

} // namespace wgl
