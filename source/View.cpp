#include "View.h"

#include "wex/Resource.h"

#include <array>
#include <vector>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

struct View::Mesh {
	ogl::Program      m_prg;
	int               m_view_mat_loc = 0;
	int               m_proj_mat_loc = 0;
	ogl::VertexArray  m_vao;
	ogl::VertexBuffer m_vertex_vbo;
	ogl::IndexBuffer  m_index_vbo;
	int               m_nElements = 0;
};

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

struct View::Rect {
	ogl::Program      m_prg;
	int               m_proj_mat_loc = 0;
	int               m_color_loc    = 0;
	ogl::VertexArray  m_vao;
	ogl::VertexBuffer m_vertex_vbo;
	int               m_nElements = 0;
};

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

static std::string_view sv( std::span<const char> sp ) {
	return std::string_view( sp.data(), sp.size() );
}

static std::string_view load_from_resource( const wchar_t * name ) {
	std::span buffer = wex::get_resource_data( ::GetModuleHandleW( nullptr ), name );
	return sv( buffer );
};

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

View::View( wex::DIBSection & dib ) 
	: m_dib( dib ) 
{
	auto t = glm::mat4{ 1 };
	t      = glm::rotate( t, glm::radians( 45.f ), glm::vec3{ 0, 0, 1 } );
	t      = glm::rotate( t, glm::radians( 45.f ), glm::vec3{ 1, 0, 0 } );
	t      = glm::translate( t, glm::vec3{ 0, 0, 130 } );
	m_eye  = t * glm::vec4{ m_eye, 1 };
	m_up   = t * glm::vec4{ m_up, 0 };

	m_start_timepoint = std::chrono::high_resolution_clock::now();

	m_mesh = std::make_unique<Mesh>();
	m_rect = std::make_unique<Rect>();
}

View::~View() {
}

void View::OnCreate( unsigned int w, unsigned int h ) {
	glClearColor( 0, 0, 1, 0 );
	glEnable( GL_DEPTH_TEST );

	// Setup the 3D program
	{
		auto vs = ogl::shader::create<ogl::VertexShader>( load_from_resource( L"vertex.vert" )  );
		auto gs = ogl::shader::create<ogl::GeometryShader>( load_from_resource( L"per_face_phong.geom" )  );
		auto fs = ogl::shader::create<ogl::FragmentShader>( load_from_resource( L"flat.frag" ) );
		auto prg = ogl::program::create( vs, gs, fs );

		m_mesh->m_proj_mat_loc = glGetUniformLocation( prg, "projMatrix" );
		m_mesh->m_view_mat_loc = glGetUniformLocation( prg, "viewMatrix" );
		m_mesh->m_prg          = std::move( prg );
	}

	// Setup the mesh VAO
	{
		m_mesh->m_vao = ogl::VertexArray::CreateAndBind();

		m_mesh->m_vertex_vbo = ogl::VertexBuffer::CreateAndBind();
		glEnableVertexAttribArray( 0 );
		glVertexAttribPointer(
			0,               // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,               // size
			GL_FLOAT,        // type
			GL_FALSE,        // normalized?
			0,               // stride
			ogl::offset( 0 ) // array buffer offset
		);

		m_mesh->m_index_vbo = ogl::IndexBuffer::CreateAndBind();
	}

	// Setup the 2D program
	{
		const char * vs_2d =
			R"(
#version 330 core

layout (location = 0) in vec2 inPosition;

uniform mat4 projMatrix;

void main()
{
	gl_Position = projMatrix * vec4( inPosition, 0.0, 1.0 );
}
		)";

		const char * fs_2d = // fragment shade
			R"(
#version 330 core

out vec4 outColor;

uniform vec4 color;

void main()
{
	outColor = color;
}
		)";

		auto vs = ogl::shader::create<ogl::VertexShader>( vs_2d );
		auto fs = ogl::shader::create<ogl::FragmentShader>( fs_2d );
		auto prg = ogl::program::create( vs, fs );

		m_rect->m_proj_mat_loc = glGetUniformLocation( prg, "projMatrix" );
		m_rect->m_color_loc    = glGetUniformLocation( prg, "color" );
		m_rect->m_prg          = std::move( prg );
	}

	// Setup the rectangle VAO
	{
		auto vao = ogl::VertexArray::CreateAndBind();
		
		auto vbo = ogl::VertexBuffer::CreateAndBind();
		glEnableVertexAttribArray( 0 );
		glVertexAttribPointer(
			0,               // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,               // size
			GL_FLOAT,        // type
			GL_FALSE,        // normalized?
			0,               // stride
			ogl::offset( 0 ) // array buffer offset
		);
		ogl::buffer::data( GL_ARRAY_BUFFER, sizeof( glm::vec2 ) * 4, GL_DYNAMIC_DRAW );

		ogl::Viewport viewport;
		glm::vec2     vmin{ static_cast<float>( viewport.x ), static_cast<float>( viewport.y ) };
		glm::vec2     vmax{ static_cast<float>( viewport.x + viewport.width ), static_cast<float>( viewport.y + viewport.height ) };

		glm::vec2 min{ 0, 0 };
		glm::vec2 max{ vmax.x / 2.0f, vmax.y / 2.0f };

		std::array rect = {
			min,
			glm::vec2{ max.x, min.y },
			glm::vec2{ min.x, max.y },
			max
		};

		ogl::buffer::sub_data( GL_ARRAY_BUFFER, 0, std::span( rect ) );

		m_rect->m_vao        = std::move( vao );
		m_rect->m_vertex_vbo = std::move( vbo );
		m_rect->m_nElements  = static_cast<int>( rect.size() );
	}

	// Setup the framebuffer where all the drawing happens
	{
		m_color_buffer = ogl::Renderbuffer::Create();
		m_color_buffer.bind();
		glRenderbufferStorage( GL_RENDERBUFFER, GL_RGBA8, w, h );

		m_depth_buffer = ogl::Renderbuffer::Create();
		m_depth_buffer.bind();
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h );

		m_frame_buffer = ogl::Framebuffer::Create();
		m_frame_buffer.bind( GL_FRAMEBUFFER );

		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_color_buffer );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_buffer );

		if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ) {
			int i = 0;
		}

		ogl::Framebuffer::unbind( GL_FRAMEBUFFER );
	}

	OnSize( w, h );
}

void View::OnDestroy() {
	// Release OpenGL resources
	m_mesh.reset();
	m_rect.reset();
}

void View::OnSize( unsigned int w, unsigned int h ) {
	m_width = w;
	m_height = h;
}

void View::Render() {
	std::chrono::time_point<std::chrono::high_resolution_clock> now          = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float>                                run_time     = std::chrono::duration_cast<std::chrono::duration<float>>( now - m_start_timepoint );
	float                                                       camera_speed = 360.0f / 10.0f;
	float                                                       camera_angle = run_time.count() * camera_speed; 

	m_frame_buffer.bind( GL_FRAMEBUFFER );
	glViewport( 0, 0, m_width, m_height );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Draw the mesh
	{
		auto t           = glm::mat4{ 1 };
		t                = glm::rotate( t, glm::radians( camera_angle ), glm::vec3{ 0, 0, 1 } );
		glm::vec3 center = m_center;
		glm::vec3 eye    = t * glm::vec4{ m_eye, 1 };
		glm::vec3 up     = t * glm::vec4{ m_up, 0 };

		auto m = m_model_matrix;

		glm::mat4 view_matrix = glm::lookAt( eye, center, up ) * m;
		glm::mat4 proj_matrix = glm::perspective( glm::radians( 45.0f ), static_cast<float>( m_width ) / static_cast<float>( m_height ), 1.f, 200.f );
	
		m_mesh->m_prg.bind();
		glUniformMatrix4fv( m_mesh->m_view_mat_loc, 1, GL_FALSE, glm::value_ptr( view_matrix ) );
		glUniformMatrix4fv( m_mesh->m_proj_mat_loc, 1, GL_FALSE, glm::value_ptr( proj_matrix ) );

		m_mesh->m_vao.bind();
		glDrawElements( GL_TRIANGLES, m_mesh->m_nElements, GL_UNSIGNED_INT, ogl::offset( 0 ) );
	}

	// Draw the rectangle
	{
		ogl::SaveState<GL_DEPTH_TEST> sd( false );
		ogl::SaveState<GL_BLEND> sb( true );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glBlendEquationSeparate( GL_FUNC_ADD, GL_MAX );

		ogl::Viewport viewport;
		glm::vec2     vmin{ static_cast<float>( viewport.x ), static_cast<float>( viewport.y ) };
		glm::vec2     vmax{ static_cast<float>( viewport.x + viewport.width ), static_cast<float>( viewport.y + viewport.height ) };

		glm::mat4 proj_matrix = glm::ortho( vmin.x, vmax.x, vmin.y, vmax.y );

		m_rect->m_prg.bind();
		glUniformMatrix4fv( m_rect->m_proj_mat_loc, 1, GL_FALSE, glm::value_ptr( proj_matrix ) );
		glm::vec4 color{ 0.0, 1.0, 0.0, 0.5 };
		glUniform4fv( m_rect->m_color_loc, 1, glm::value_ptr( color ) );

		m_rect->m_vao.bind();
		glDrawArrays( GL_TRIANGLE_STRIP, 0, m_rect->m_nElements );
	}

	// Copy the content of the color framebuffer to an image
	glReadBuffer( GL_COLOR_ATTACHMENT0 );
	glReadPixels( 0, 0, m_dib.Width(), m_dib.Height(), GL_BGRA, GL_UNSIGNED_BYTE, m_dib.Bits() );
	
	ogl::Framebuffer::unbind( GL_FRAMEBUFFER );
}

void View::Update() {
	// Center the mesh and scale it to fit the view
	{
		glm::vec3 pmin;
		glm::vec3 pmax;
		bool      bFirst = true;

		for( const glm::vec3 & p : m_vertices ) {
			if( bFirst ) {
				pmin   = p;
				pmax   = p;
				bFirst = false;
			}
			else {
				using std::max;
				using std::min;
				pmin = min( pmin, p );
				pmax = max( pmax, p );
			}
		}
		glm::vec3 center = ( pmax + pmin ) * 0.5f;

		auto m  = glm::mat4{ 1 };
		m       = glm::rotate( m, glm::radians( 90.f ), glm::vec3{ 1, 0, 0 } );
		float s = 90.f / std::max( pmax.x, std::max( pmax.y, pmax.z ) );
		m       = glm::scale( m, glm::vec3( s, s, s ) );
		m       = glm::translate( m, -center );

		m_model_matrix = m;
	}

	// Update the mesh with new data
	{
		m_mesh->m_vertex_vbo.bind();
		ogl::buffer::data( GL_ARRAY_BUFFER, std::span( m_vertices ), GL_STATIC_DRAW );

		m_mesh->m_index_vbo.bind();
		ogl::buffer::data( GL_ELEMENT_ARRAY_BUFFER, std::span( m_triangles ), GL_STATIC_DRAW );

		m_mesh->m_nElements = static_cast<int>( m_triangles.size() ) * 3;
	}
}

void View::SetMesh( std::vector<glm::vec3> vertices, std::vector<std::array<int, 3>> triangles ) {
	m_vertices = std::move( vertices );
	m_triangles = std::move( triangles );

	Update();
}
