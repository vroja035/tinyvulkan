/*
	
	Renders a simple triangle on screen.

*/

#pragma once

#include "tve_descriptors.h"
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

		TveWindow tveWindow{ WIDTH, HEIGHT, "tinyvulkan" };
		TveDevice tveDevice{ tveWindow };
		TveRenderer tveRenderer{ tveWindow, tveDevice };

		std::unique_ptr<TveDescriptorPool> globalPool{};
		TveGameObject::Map gameObjects;
		
	};
} // namespace tve
