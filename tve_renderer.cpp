#include "tve_renderer.h"

//std
#include <cassert>
#include <stdexcept>
#include <array>

namespace tve {

	TveRenderer::TveRenderer(TveWindow& window, TveDevice& device) 
		: tveWindow{ window }, tveDevice{ device } 
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	TveRenderer::~TveRenderer() {
		freeCommandBuffers();
	}

	void TveRenderer::recreateSwapChain() {
		auto extent = tveWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = tveWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(tveDevice.device());

		if (tveSwapChain == nullptr) {
			tveSwapChain = std::make_unique<TveSwapChain>(tveDevice, extent);
		}
		else {
			std::shared_ptr<TveSwapChain> oldSwapChain = std::move(tveSwapChain);
			tveSwapChain = std::make_unique<TveSwapChain>(tveDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*tveSwapChain.get())) {
				throw std::runtime_error("Swap chain image (or depth) format has changed!");
			}
		}
	}

	void TveRenderer::createCommandBuffers() {
		commandBuffers.resize(TveSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; //can be submitted but cannot be called by other cmd buffers
		allocInfo.commandPool = tveDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(tveDevice.device(), &allocInfo,
			commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers!");
		}
	}

	void TveRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(
			tveDevice.device(),
			tveDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer TveRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Cannot call beginFrame while already in progress!");

		VkResult result = tveSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recoding command buffer!");
		}
		return commandBuffer;
	}

	void TveRenderer::endFrame()
	{
		assert(isFrameStarted && "Cannot call endFrame while not in progress!");
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer!");
		}

		auto result = tveSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_SUBOPTIMAL_KHR ||
			tveWindow.hasWindowResized())
		{

			tveWindow.resetWindowResizedFlag();
			recreateSwapChain();

		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % TveSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void TveRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cannot call beginSwapChainRenderPass while not in progress!");
		assert(commandBuffer == getCurrentCommandBuffer() &&
			"Cannot begin render pass on command buffer from a different frame!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = tveSwapChain->getRenderPass();
		renderPassInfo.framebuffer = tveSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = tveSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(tveSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(tveSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, tveSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}
	
	void TveRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cannot call endSwapChainRenderPass while not in progress!");
		assert(commandBuffer == getCurrentCommandBuffer() &&
			"Cannot end render pass on command buffer from a different frame!");

		vkCmdEndRenderPass(commandBuffer);
	}


} // namespace tve