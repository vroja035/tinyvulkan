#pragma once

#include "tve_device.h"
#include "tve_game_object.h"
#include "tve_pipeline.h"


//std
#include <memory>
#include <vector>


namespace tve {
	class SimpleRenderSystem {
	public:

		SimpleRenderSystem(TveDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator = (const SimpleRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<TveGameObject>& gameObjects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
		

		
		TveDevice& tveDevice;
		

		std::unique_ptr<TvePipeline> tvePipeline;
		VkPipelineLayout pipelineLayout;	

	};
} // namespace tve

