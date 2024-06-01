/*
	Manages swapchain, command buffer cycles, and drawing frames.

	Creates command buffers to submit commands to which then get passed on to the device queue
	to get executed.

	- Command buffers allow a sequence of commands to be recorded once and reused.
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
