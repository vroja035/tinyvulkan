/*
	
	Billboard: flat 2d object embedded in 2d world; always facing the camera.
	- used for healthbars or particle systems

	Screen Alightned: affected by camera rotation but not position
	Viewpoint Oriented: affected by both rotation and position

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
	class PointLightSystem {
	public:

		PointLightSystem(TveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator = (const PointLightSystem&) = delete;

		void render(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);



		TveDevice& tveDevice;


		std::unique_ptr<TvePipeline> tvePipeline;
		VkPipelineLayout pipelineLayout;

	};
} // namespace tve
