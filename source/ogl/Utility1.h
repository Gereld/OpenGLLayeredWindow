#pragma once

namespace ogl {

inline void get( GLenum pname, GLboolean * params ) {
	glGetBooleanv( pname, params );
}

inline void get( GLenum pname, GLfloat * params ) {
	glGetFloatv( pname, params );
}

inline void get( GLenum pname, GLint * params ) {
	glGetIntegerv( pname, params );
}

inline void get( GLenum pname, GLenum * params ) {
	glGetIntegerv( pname, reinterpret_cast<GLint *>( params ) );
}

class SaveAnyState {
	GLenum m_state;
	bool   m_bValue;

public:
	SaveAnyState( GLenum state ) {
		m_state  = state;
		m_bValue = glIsEnabled( m_state ) != 0;
	}

	SaveAnyState( GLenum state, bool bNewValue ) {
		m_state  = state;
		m_bValue = glIsEnabled( m_state ) != 0;
		if( bNewValue ) {
			glEnable( m_state );
		}
		else {
			glDisable( m_state );
		}
	}

	~SaveAnyState() {
		if( m_bValue ) {
			glEnable( m_state );
		}
		else {
			glDisable( m_state );
		}
	}
};

template<GLenum STATE>
class SaveState {
	bool m_bValue;

public:
	SaveState() {
		m_bValue = glIsEnabled( STATE ) != 0;
	}

	SaveState( bool bNewValue ) {
		m_bValue = glIsEnabled( STATE ) != 0;
		if( bNewValue ) {
			glEnable( STATE );
		}
		else {
			glDisable( STATE );
		}
	}

	~SaveState() {
		if( m_bValue ) {
			glEnable( STATE );
		}
		else {
			glDisable( STATE );
		}
	}
};

struct Viewport {
	int x;
	int y;
	int width;
	int height;

	Viewport() {
		glGetIntegerv( GL_VIEWPORT, reinterpret_cast<GLint *>( this ) );
	}
};

struct Transparency : public SaveState<GL_BLEND> {
	Transparency( bool bEnable = true )
		: SaveState<GL_BLEND>( bEnable ) {
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}
};

} // namespace ogl
