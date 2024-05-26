#include "tve_window.h"

#include <stdexcept>
namespace tve {
	
	TveWindow::TveWindow(int w, int h, std::string name) : 
		width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}

	TveWindow::~TveWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void TveWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}

	void TveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface!");
		}
	}

} //namespace tve