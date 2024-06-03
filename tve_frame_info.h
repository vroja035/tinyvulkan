#pragma once

#include "tve_camera.h"
#include "tve_game_object.h"

// lib
#include <vulkan/vulkan.h>

namespace tve {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		TveCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		TveGameObject::Map& gameObjects;
	};
} // namespace tve