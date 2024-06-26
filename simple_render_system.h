/*
	Manages pipeline and pipeline layout, and rendering game objects.

	Can have multiple rendering systems.

	Not all game objects are compatible with all systems.
*/

#pragma once

#include "tve_camera.h"
#include "tve_device.h"
#include "tve_game_object.h"
#include "tve_pipeline.h"
#include "tve_frame_info.h"


//std
#include <memory>
#include <vector>


namespace tve {
	class SimpleRenderSystem {
	public:

		SimpleRenderSystem(TveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator = (const SimpleRenderSystem&) = delete;

		void renderGameObjects(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
		

		
		TveDevice& tveDevice;
		

		std::unique_ptr<TvePipeline> tvePipeline;
		VkPipelineLayout pipelineLayout;	

	};
} // namespace tve

