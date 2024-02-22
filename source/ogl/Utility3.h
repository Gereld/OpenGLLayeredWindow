#pragma once

#include "Utility1.h"

#include <span>
#include <string_view>

namespace ogl {

template<typename This>
struct Resource {
	GLuint m_id = 0;

	Resource() {
	}

	Resource( const Resource & )             = delete;
	Resource & operator=( const Resource & ) = delete;

	Resource( Resource && rhs ) noexcept {
		m_id     = rhs.m_id;
		rhs.m_id = 0;
	}

	~Resource() {
		static_cast<This *>( this )->destroy();
	}

	This & operator=( Resource && rhs ) noexcept {
		using std::swap;
		swap( m_id, rhs.m_id );

		return static_cast<This &>( *this );
	}

	operator GLuint() const {
		return m_id;
	}

	static This Create() {
		This t;
		t.create();
		return t;
	}

	static This CreateAndBind() {
		This t;
		t.create();
		t.bind();
		return t;
	}
};

enum class TextureTarget : GLenum {
	T2D    = GL_TEXTURE_2D,
	Buffer = GL_TEXTURE_BUFFER,
};

template<TextureTarget TARGET>
struct Texture : Resource<Texture<TARGET>> {
	using Resource<Texture<TARGET>>::Resource;

	void create() {
		glGenTextures( 1, &this->m_id );
	}

	void destroy() {
		if( this->m_id != 0 ) {
			glDeleteTextures( 1, &this->m_id );
			this->m_id = 0;
		}
	}

	void bind() {
		glBindTexture( static_cast<GLenum>( TARGET ), this->m_id );
	}

	static void unbind() {
		glBindTexture( static_cast<GLenum>( TARGET ), 0 );
	}
};

using Texture2D     = Texture<TextureTarget::T2D>;
using BufferTexture = Texture<TextureTarget::Buffer>;

struct Sampler : Resource<Sampler> {
	using Resource<Sampler>::Resource;

	void create() {
		glGenSamplers( 1, &m_id );
	}

	void destroy() {
		if( m_id != 0 ) {
			glDeleteSamplers( 1, &m_id );
			m_id = 0;
		}
	}

	void bind( GLuint unit ) {
		glBindSampler( unit, m_id );
	}

	static void unbind( GLuint unit ) {
		glBindSampler( unit, 0 );
	}
};

enum class BufferTarget : GLuint {
	Vertex  = GL_ARRAY_BUFFER,
	Index   = GL_ELEMENT_ARRAY_BUFFER,
	Texture = GL_TEXTURE_BUFFER,
};

template<BufferTarget TARGET>
struct Buffer : public Resource<Buffer<TARGET>> {
	using Resource<Buffer<TARGET> >::Resource;

	void create() {
		glGenBuffers( 1, &this->m_id );
	}

	void destroy() {
		if( this-> m_id != 0 ) {
			glDeleteBuffers( 1, &this->m_id );

			this->m_id = 0;
		}
	}

	constexpr BufferTarget target() const {
		return TARGET;
	}

	void bind() {
		glBindBuffer( static_cast<GLuint>( TARGET ), this->m_id );
	}

	static void unbind() {
		glBindBuffer( static_cast<GLuint>( TARGET ), 0 );
	}
};

using VertexBuffer  = Buffer<BufferTarget::Vertex>;
using IndexBuffer   = Buffer<BufferTarget::Index>;
using TextureBuffer = Buffer<BufferTarget::Texture>;

struct VertexArray : public Resource<VertexArray> {
	using Resource<VertexArray>::Resource;

	void create() {
		glGenVertexArrays( 1, &m_id );
	}

	void destroy() {
		if( m_id != 0 ) {
			glDeleteVertexArrays( 1, &m_id );
			m_id = 0;
		}
	}

	void bind() {
		glBindVertexArray( m_id );
	}

	static void unbind() {
		glBindVertexArray( 0 );
	}
};

struct Program : public Resource<Program> {
	using Resource<Program>::Resource;

	void create() {
		m_id = glCreateProgram();
	}

	void destroy() {
		if( m_id != 0 ) {
			glDeleteProgram( m_id );
		}
		m_id = 0;
	}

	void bind() {
		glUseProgram( m_id );
	}

	static void unbind() {
		glUseProgram( 0 );
	}
};

enum class ShaderType : GLenum {
	Vertex   = GL_VERTEX_SHADER,
	Fragment = GL_FRAGMENT_SHADER,
	Geometry = GL_GEOMETRY_SHADER,
};

template<ShaderType TYPE>
struct Shader : public Resource<Shader<TYPE>> {
	using Resource<Shader<TYPE>>::Resource;
	using Resource<Shader<TYPE>>::m_id;

	void create() {
		m_id = glCreateShader( static_cast<GLenum>( TYPE ) );
	}

	void destroy() {
		if( m_id != 0 ) {
			glDeleteShader( m_id );
		}
		m_id = 0;
	}
};

using VertexShader   = Shader<ShaderType::Vertex>;
using FragmentShader = Shader<ShaderType::Fragment>;
using GeometryShader = Shader<ShaderType::Geometry>;

struct Framebuffer : public Resource<Framebuffer> {
	using Resource<Framebuffer>::Resource;

	void create() {
		glGenFramebuffers( 1, &this->m_id );
	}

	void destroy() {
		if( this->m_id != 0 ) {
			glDeleteFramebuffers( 1, &this->m_id );
			this->m_id = 0;
		}
	}

	void bind( GLuint target ) {
		glBindFramebuffer( target, this->m_id );
	}

	static void unbind( GLuint target ) {
		glBindFramebuffer( target, 0 );
	}
};

struct Renderbuffer : public Resource<Renderbuffer> {
	using Resource<Renderbuffer>::Resource;

	void create() {
		glGenRenderbuffers( 1, &m_id );
	}

	void destroy() {
		if( m_id != 0 ) {
			glDeleteRenderbuffers( 1, &m_id );
			m_id = 0;
		}
	}

	void bind() {
		glBindRenderbuffer( GL_RENDERBUFFER, m_id );
	}

	static void unbind() {
		glBindRenderbuffer( GL_RENDERBUFFER, 0 );
	}
};

// ***********************************************************************************************

constexpr GLvoid * offset( std::integral auto i ) {
	return (char *) nullptr + ( i );
}

namespace buffer {

inline void data( GLenum target, size_t size, GLenum usage ) {
	glBufferData( target, size, nullptr, usage );
}

template<class ElementType, size_t Extend>
void data( GLenum target, std::span<ElementType, Extend> data, GLenum usage ) {
	glBufferData( target, data.size_bytes(), data.data(), usage );
}

template<class ElementType, size_t Extend>
void sub_data( GLenum target, GLintptr offset, std::span<ElementType, Extend> data ) {
	glBufferSubData( target, offset, data.size_bytes(), data.data() );
}

} // namespace buffer

namespace shader {

inline void source( GLuint shader, std::string_view src ) {
	const char * strings[1] = { src.data() };
	int          lengths[1] = { static_cast<int>( src.size() ) };
	glShaderSource( shader, 1, strings, lengths );
}

template<class Shader>
Shader create( std::string_view src ) {
	auto s = Shader::Create();
	source( s, src );
	glCompileShader( s );

	int status = 0;
	glGetShaderiv( s, GL_COMPILE_STATUS, &status );
	return s;
}

} // namespace shader

namespace program {

inline void link( GLuint prg ) {
	glLinkProgram( prg );

	int status = 0;
	glGetProgramiv( prg, GL_LINK_STATUS, &status );
}

template<ShaderType... TYPE>
void attach( GLuint prg, const Shader<TYPE> &... ts ) {
	( glAttachShader( prg, ts ), ... );
}

template<ShaderType... TYPE>
auto create( const Shader<TYPE> &... ts ) {
	auto prg = Program::Create();
	attach( prg, ts... );
	link( prg );
	prg.bind();
	return prg;
}
	
} // namespace program

} // namespace ogl
