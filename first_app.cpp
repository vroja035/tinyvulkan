#include "first_app.h"

#include "keyboard_movement_controller.h"
#include "tve_camera.h"
#include "simple_render_system.h"
#include "tve_buffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //for vulkan
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <cassert>
#include <chrono>
#include <stdexcept>
#include <array>

namespace tve {

	struct GlobalUbo {
		alignas(16) glm::mat4 projectionView{ 1.f };
		alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
	};

	FirstApp::FirstApp() {
		globalPool = TveDescriptorPool::Builder(tveDevice)
			.setMaxSets(TveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, TveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run() {

		std::vector<std::unique_ptr<TveBuffer>> uboBuffers(TveSwapChain::MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<TveBuffer>(
				tveDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT //| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = TveDescriptorSetLayout::Builder(tveDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(TveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			TveDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ tveDevice, tveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        TveCamera camera{};

        auto viewerObject = TveGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!tveWindow.shouldClose()) {
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(tveWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = tveRenderer.getAspectRatio();
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
			
			if (auto commandBuffer = tveRenderer.beginFrame()) {

				int frameIndex = tveRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex]
				};
				
				//	update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render
				tveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
				tveRenderer.endSwapChainRenderPass(commandBuffer);
				tveRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(tveDevice.device());
	}

	void FirstApp::loadGameObjects() {
		std::shared_ptr<TveModel> tveModel = TveModel::
			createModelFromFile(tveDevice, "models/smooth_vase.obj");

        auto obj = TveGameObject::createGameObject();
        obj.model = tveModel;
        //obj.transform.translation = { .0f, .0f, 2.5f };
		obj.transform.translation = { -.5f, .5f, 2.5f };
        //obj.transform.scale = glm::vec3(2.f);
		obj.transform.scale = {3.f, 1.5f, 3.f};
		gameObjects.push_back(std::move(obj));

		
		tveModel = TveModel::createModelFromFile(tveDevice, "models/flat_vase.obj");

		obj = TveGameObject::createGameObject();
		obj.model = tveModel;
		obj.transform.translation = { .5f, .5f, 2.5f };
		//obj.transform.scale = glm::vec3(2.f);
		obj.transform.scale = { 3.f, 1.5f, 3.f };
		gameObjects.push_back(std::move(obj));
	}

} // namespace tve