#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace tve {

	class TveWindow {

	public:
		TveWindow(int w, int h, std::string name);
		~TveWindow();

		TveWindow(const TveWindow&) = delete;
		TveWindow& operator = (const TveWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }
	private:
		
		void initWindow();
		
		const int width;
		const int height;

		std::string windowName;
		GLFWwindow* window;
	};
} //namespace tve
