/*
	
	Renders a simple triangle on screen.
	
	Creates command buffers to submit commands to which then get passed on to the device queue
	to get executed.

	- Command buffers allow a sequence of commands to be recorded once and reused.

*/

#pragma once

#include "tve_device.h"
#include "tve_game_object.h"
#include "tve_renderer.h"
#include "tve_window.h"

//std
#include <memory>
#include <vector>


namespace tve {
	class FirstApp {
	public:
		static constexpr int WIDTH = 1600;
		static constexpr int HEIGHT = 1200;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator = (const FirstApp&) = delete;

		void run();

	private:
		void loadGameObjects();

		TveWindow tveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		TveDevice tveDevice{ tveWindow };
		TveRenderer tveRenderer{ tveWindow, tveDevice };

		std::vector<TveGameObject> gameObjects;
		
	};
} // namespace tve
