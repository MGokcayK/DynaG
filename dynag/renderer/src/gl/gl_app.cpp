#include "gl_app.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <iostream>
#include <array>
#include <chrono>

#include "gl/gl_window.h"

namespace OpenGL
{
	Application::Application(const unsigned int _width,
							const unsigned int _height,
							const char* _title)
	{
		if (m_window == nullptr)
		{
			m_window = std::make_unique<Window>(_width, _height, _title);
		}
		api = ApplicationAPI::kOpenGL;
	}

	Application::~Application()
	{
	}
	
	Model* Application::createModel(const std::string& path,
		std::string vertex_shader_file_path,
		std::string fragment_shader_file_path)
	{
		return new Model(path, vertex_shader_file_path, fragment_shader_file_path);
	}
}