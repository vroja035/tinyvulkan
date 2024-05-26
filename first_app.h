#pragma once

#include "tve_window.h"
#include "tve_pipeline.h"
#include "tve_device.h"


namespace tve {
	class FirstApp {
	public:
		static constexpr int WIDTH = 1280;
		static constexpr int HEIGHT = 720;

		void run();

	private:
		TveWindow tveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		TveDevice tveDevice{ tveWindow };
		TvePipeline tvePipeline{
			tveDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			TvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
		
	};
} //namespace tve
