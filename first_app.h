#pragma once

#include "tve_window.h"

namespace tve {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();
	
	private:
		TveWindow tveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
	};
}
