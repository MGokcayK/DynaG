#include "vk_window.h"
//#include "user_input.h"

#include <stdexcept>
#include <iostream>

namespace Vulkan
{
	Window::Window(const unsigned int _width,
		const unsigned int _height,
		const char* _title)
	{
		fScrWidth = (float)_width;
		fScrHeight = (float)_height;
		m_windowName = _title;

		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		wWindow = glfwCreateWindow(_width, _height, _title, NULL, NULL);
		if (wWindow == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
		}

		// Create camera.
		cmCamera = new Camera(glm::vec3(5.415f, 0.2f, 30.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		fLastX = _width / 2.0f;
		fLastY = _height / 2.0f;

		// Set Input Mode for mouse buttons.
		glfwSetInputMode(wWindow, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

		// Set callbacks functions to the window and window pointer to OpenGL.
		glfwSetWindowUserPointer(wWindow, this);
		glfwSetFramebufferSizeCallback(wWindow, WindowBase::staticFrameBufferSizeCallback);
		glfwSetCursorPosCallback(wWindow, WindowBase::staticMouseCallback);
		glfwSetScrollCallback(wWindow, WindowBase::staticScrollCallback);

		// Create gui.
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui_ImplGlfw_InitForVulkan(wWindow, true);

		// use vec4 for handling std140 format for uniform buffer objects
		vLightPosition = glm::vec4(0.0f, 7500.0f, 0.0f, 0.0f);
		glm::vec4 light_ambient = glm::vec4(0.8f, 0.8f, 0.8f, 0.0f);
		glm::vec4 light_diffuse = glm::vec4(255.0 / 255.0f, 241.0 / 255.0f, 242.0 / 255.0f, 0.0f);
		glm::vec4 light_specular = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);

		glm::vec4 fog_color = glm::vec4(0.74f, 0.35f, 0.51f, 0.4f);
		glm::vec4 density_grad = glm::vec4(0.002f, 5.0f, 0.0, 0.0); // density & grad in first 2 elements, other for padding
	}

	VkExtent2D Window::getExtend()
	{
		return { static_cast<uint32_t>(fScrWidth), static_cast<uint32_t>(fScrHeight) };
	}

	bool Window::wasWindowResize()
	{
		return m_frameBufferResized;
	}

	void Window::resetWindowResizedFlag()
	{
		m_frameBufferResized = false;
	}

	void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, wWindow, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create windows surface");
		}
	}

	void Window::setKeyCallback(GLFWkeyfun keycallback)
	{
		glfwSetKeyCallback(wWindow, keycallback);
	}

	void Window::frameBufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		auto window_ = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		window_->m_frameBufferResized = true;
		window_->fScrWidth = (float)width;
		window_->fScrHeight = (float)height;
	}

	/*
	void Window::cursorCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (Window::windowFocused)
		{
			float xchange = (float)xpos - cursorXLast;
			float ychange = (float)ypos - cursorYLast;

			// make sure that cursor is not jump when the cursor move on window from outside into frame
			cursorXchange = (fabs(xchange) < 30.0f) ? xchange : cursorXchange;
			// when dealing with opengl order, y should be inverted, so cursor motion should also be inverted.
			cursorYchange = (fabs(ychange) < 30.0f) ? -ychange : cursorYchange; 

			cursorXLast = (float)xpos;
			cursorYLast = (float)ypos;
		}
		else
		{
			cursorXchange = 0.0f;
			cursorYchange = 0.0f;

			cursorXLast = 0.0f;
			cursorYLast = 0.0f;
		}
	}

	void Window::resetCursorMotion()
	{
		cursorXchange = 0.0f;
		cursorYchange = 0.0f;
	}
	*/
}