#pragma once

#include "tve_camera.h"

// lib
#include <vulkan/vulkan.h>

namespace tve {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		TveCamera& camera;
	};
} // namespace tve