#include "first_app.h"

namespace tve {
	void FirstApp::run() {
		while (!tveWindow.shouldClose()) {
			glfwPollEvents();
		}
	}
} // namespace tve