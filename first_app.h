#pragma once

#include "tve_window.h"
#include "tve_pipeline.h"
#include "tve_device.h"
#include "tve_swap_chain.h"

#include <memory>
#include <vector>


namespace tve {
	class FirstApp {
	public:
		static constexpr int WIDTH = 1280;
		static constexpr int HEIGHT = 720;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator = (const FirstApp&) = delete;

		void run();

	private:

		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		TveWindow tveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		TveDevice tveDevice{ tveWindow };
		TveSwapChain tveSwapChain{ tveDevice, tveWindow.getExtent() };
		std::unique_ptr<TvePipeline> tvePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		
	};
} //namespace tve
