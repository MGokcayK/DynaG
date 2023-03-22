#ifndef VK_WINDOW
#define VK_WINDOW

#include <vulkan/vulkan.h>
#include <string>

#include "base/window.h"

namespace Vulkan
{
	class Window : public WindowBase
	{
	public:
		Window(const unsigned int _width,
			const unsigned int _height,
			const char* _title);

		VkExtent2D getExtend();
		bool wasWindowResize();
		void resetWindowResizedFlag();
		
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

		void setKeyCallback(GLFWkeyfun keycallback);

		//inline static bool windowFocused = false;
		//inline static float cursorXchange{ 0.0f };
		//inline static float cursorXLast{ 0.0f };
		//inline static float cursorYchange{ 0.0f };
		//inline static float cursorYLast{ 0.0f };
		//static void resetCursorMotion();
	private:
		virtual void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
		//void cursorCallback(GLFWwindow* window, double xpos, double ypos);

		
		bool m_frameBufferResized = false;
		std::string m_windowName;
	};
}

#endif // !VK_WINDOW