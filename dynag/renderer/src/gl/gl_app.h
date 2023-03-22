#ifndef GL_APPLICATION
#define GL_APPLICATION

#include "base/app.h"
#include "gl_window.h"

namespace OpenGL
{
	class Application : public ApplicationBase
	{
	public:

		Application(const unsigned int _width,
					const unsigned int _height,
					const char* _title);
		~Application();

		Model* createModel(const std::string& path,
			std::string vertex_shader_file_path,
			std::string fragment_shader_file_path);
	
	private:
	};
}

#endif // !GL_APPLICATION