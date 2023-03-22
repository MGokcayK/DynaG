#ifndef VK_APPLICATION
#define VK_APPLICATION

#include "base/app.h"

#include "vk_renderer.h"
#include "vk_model.h"

//#include "game_object.h"
//#include "descriptor.h"

#include <memory>
#include <vector>

namespace Vulkan
{
	class Application : public ApplicationBase
	{
	public:

		Application(const unsigned int _width,
					const unsigned int _height,
					const char* _title);
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		//void run();

		Model* createModel(const std::string& path,
			std::string vertex_shader_file_path,
			std::string fragment_shader_file_path);

	private:
		//void loadGameObjects();
		//static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		std::unique_ptr<Device> m_device{};
		std::unique_ptr<Renderer> m_renderer{};

		//GameObject::Map m_gameObjects;
		std::unique_ptr<DescriptorPool> m_globalPool{};
		std::unique_ptr<DescriptorSetLayout> m_globalSetLayout{};
		std::unique_ptr<DescriptorSetLayout> m_materialSetLayout{};
	};
}

#endif