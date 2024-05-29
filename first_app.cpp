#include "first_app.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //for vulkan
#include <glm/glm.hpp>

//std
#include <cassert>
#include <stdexcept>
#include <array>

namespace tve {

	struct SimplePushConstantData {
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	FirstApp::FirstApp() {
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	FirstApp::~FirstApp() {
		vkDestroyPipelineLayout(tveDevice.device(), pipelineLayout, nullptr);
	}

	void FirstApp::run() {
		while (!tveWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(tveDevice.device());
	}

	void FirstApp::loadModels() {
		std::vector<TveModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		tveModel = std::make_unique<TveModel>(tveDevice, vertices);
		/*std::vector<TveModel::Vertex> vertices{};
		sierpinski(vertices, 5, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });
		tveModel = std::make_unique<TveModel>(tveDevice, vertices);*/
	}

	void FirstApp::createPipelineLayout() {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		
		if (vkCreatePipelineLayout(tveDevice.device(),
			&pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}

	}

	void FirstApp::createPipeline() {
		assert(tveSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
			
		TvePipeline::defaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.renderPass = tveSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		tvePipeline = std::make_unique<TvePipeline>(
			tveDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig);
	}

	void FirstApp::recreateSwapChain() {
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
			tveSwapChain = std::make_unique<TveSwapChain>(tveDevice, extent, std::move(tveSwapChain));
			if (tveSwapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		//TODO: check if renderpasses are compatible
		createPipeline();
	}

	void FirstApp::createCommandBuffers() {
		commandBuffers.resize(tveSwapChain->imageCount());

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

	void FirstApp::freeCommandBuffers() {
		vkFreeCommandBuffers(
			tveDevice.device(), 
			tveDevice.getCommandPool(), 
			static_cast<uint32_t>(commandBuffers.size()), 
			commandBuffers.data());
		commandBuffers.clear();
	}
	
	void FirstApp::recordCommandBuffer(int imageIndex) {

		static int frame = 0;
		frame = (frame + 1) % 1000;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recoding command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = tveSwapChain->getRenderPass();
		renderPassInfo.framebuffer = tveSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = tveSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(tveSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(tveSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, tveSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		tvePipeline->bind(commandBuffers[imageIndex]);
		tveModel->bind(commandBuffers[imageIndex]);

		for (int j = 0; j < 4; j++) {
			SimplePushConstantData push{};
			push.offset = { -0.5f + frame * 0.02f, -0.4f + j * 0.25f };
			push.color = { 0.0f, 0.0f, 0.2f + 0.2f * j };

			vkCmdPushConstants(
				commandBuffers[imageIndex], 
				pipelineLayout, 
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
				0, 
				sizeof(SimplePushConstantData), &push);

			tveModel->draw(commandBuffers[imageIndex]);
		}

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer!");
		}
	}

	void FirstApp::drawFrame() {
		uint32_t imageIndex;
		
		VkResult result = tveSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		recordCommandBuffer(imageIndex);

		result = tveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_SUBOPTIMAL_KHR || 
			tveWindow.hasWindowResized()) 
		{
			
			tveWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;

		}
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image!");
		}
	}

	void FirstApp::sierpinski(
		std::vector<TveModel::Vertex>& vertices,
		int depth,
		glm::vec2 left,
		glm::vec2 right,
		glm::vec2 top) {
		if (depth <= 0) {
			vertices.push_back({ top });
			vertices.push_back({ right });
			vertices.push_back({ left });
		}
		else {
			auto leftTop = 0.5f * (left + top);
			auto rightTop = 0.5f * (right + top);
			auto leftRight = 0.5f * (left + right);
			sierpinski(vertices, depth - 1, left, leftRight, leftTop);
			sierpinski(vertices, depth - 1, leftRight, right, rightTop);
			sierpinski(vertices, depth - 1, leftTop, rightTop, top);
		}
	}

} // namespace tve