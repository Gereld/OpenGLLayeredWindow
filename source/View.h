#pragma once

#include "gl.h"
#include "wex/DIBSection.h"

#include <chrono>
#include <filesystem>
#include <glm/glm.hpp>
#include <memory>

class View {
	wex::DIBSection & m_dib;

	struct Mesh;
	struct Rect;
	std::unique_ptr<Mesh> m_mesh;
	std::unique_ptr<Rect> m_rect;

	unsigned int m_width  = 0;
	unsigned int m_height = 0;
	
	glm::vec3                                                   m_center{ 0, 0, 0 };
	glm::vec3                                                   m_eye{ 0, 0, 1 };
	glm::vec3                                                   m_up{ 0, 1, 0 };
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start_timepoint;

	std::vector<glm::vec3>          m_vertices;
	std::vector<std::array<int, 3>> m_triangles;
	glm::mat4						m_model_matrix;

	ogl::Renderbuffer m_color_buffer;
	ogl::Renderbuffer m_depth_buffer;
	ogl::Framebuffer  m_frame_buffer;

public:
	View( wex::DIBSection & dib );
	~View();

	void OnCreate( unsigned int w, unsigned int h );
	void OnDestroy();
	void OnSize( unsigned int w, unsigned int h );
	void Render();
	void Update();

	void SetMesh( std::vector<glm::vec3> vertices, std::vector<std::array<int, 3>> triangles ); 
};
