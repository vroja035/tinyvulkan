/*
	
	Renders a simple triangle on screen.
	
	Creates command buffers to submit commands to which then get passed on to the device queue
	to get executed.

	- Command buffers allow a sequence of commands to be recorded once and reused.

*/

#pragma once

#include "tve_device.h"
#include "tve_game_object.h"
#include "tve_pipeline.h"
#include "tve_swap_chain.h"
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
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);

		//recursive func to render sierpinski's triangle
		void sierpinski(
			std::vector<TveModel::Vertex>& vertices,
			int depth,
			glm::vec2 left,
			glm::vec2 right,
			glm::vec2 top);

		TveWindow tveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		TveDevice tveDevice{ tveWindow };
		std::unique_ptr<TveSwapChain> tveSwapChain;
		std::unique_ptr<TvePipeline> tvePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<TveGameObject> gameObjects;
		
	};
} // namespace tve
