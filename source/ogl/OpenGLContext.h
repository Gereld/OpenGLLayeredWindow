#pragma once

namespace ogl {

class OpenGLContext {
public:
	virtual ~OpenGLContext() = default;

	virtual void MakeCurrent() = 0;
	virtual void SwapBuffers() = 0;
};

} // namespace ogl
