/*
	Manages swapchain, command buffer cycles, and drawing frames.

	Creates command buffers to submit commands to which then get passed on to the device queue
	to get executed.

	- Command buffers allow a sequence of commands to be recorded once and reused.

	Lighting model: a simplification of the Light Transport Equation used to calculate the intensity
	of reflected light at a given point on a surface.

	Diffuse Model
		- Makes assumption that the surfrace reflects incoming light equally in all direction
		- good for matte, non metallic surfaces; opposite of a mirror surface
		- obey Lamberts Cosine Law : lightIntensity is proportional to cos (angle between surface normal 
		and direction to lgith source)

	Directional Lighting: uses parallel light rays
	 - simulates lgiht source that is inf away from the object

	Indirect illumination
	- ambient lighting

	Vertex Normals: Smooth shading
	 - Normals perpendicular to "implied" curved surfaced

	 Face Normals: Flat shading
	 - Multiple normals per vertex position

	 Uniform buffer: way to provide arbitrary read-only data to shaders
		- reading can be slower
		- reqruire additional setup (buffer + descriptor sets)
		- binding descriptor set has some overhead

	Point lighting: relative positioning between light source and surface model matter
	- can hit a flat surface at different angles
	- point lights are attenuated -> intensity proportional to 1/distance^2 (further away, less light)
		- can be controlled by constant, linear, quadratic params
*/
#pragma once

#include "tve_device.h"
#include "tve_swap_chain.h"
#include "tve_window.h"

//std
#include <cassert>
#include <memory>
#include <vector>


namespace tve {
	class TveRenderer {
	public:

		TveRenderer(TveWindow& tveWindow, TveDevice& tveDevice);
		~TveRenderer();

		TveRenderer(const TveRenderer&) = delete;
		TveRenderer& operator = (const TveRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return tveSwapChain->getRenderPass(); }
		float getAspectRatio() const { return tveSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress!");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress!");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		TveWindow& tveWindow;
		TveDevice& tveDevice;
		std::unique_ptr<TveSwapChain> tveSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };

	};
} // namespace tve
