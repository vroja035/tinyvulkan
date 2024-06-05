#include "first_app.h"

#include "keyboard_movement_controller.h"
#include "tve_camera.h"
#include "simple_render_system.h"
#include "point_light_system.h"
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
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f }; // w is intensity
		glm::vec3 lightPosition{ -1.f };
		alignas(16) glm::vec4 lightColor{ 1.f }; // w is light intensity
		
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
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(TveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			TveDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ 
			tveDevice, 
			tveRenderer.getSwapChainRenderPass(), 
			globalSetLayout->getDescriptorSetLayout()};
		PointLightSystem pointLightSystem{
			tveDevice,
			tveRenderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout() };
        TveCamera camera{};

        auto viewerObject = TveGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
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
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);
			
			if (auto commandBuffer = tveRenderer.beginFrame()) {

				int frameIndex = tveRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};
				
				//	update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render
				tveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
				tveRenderer.endSwapChainRenderPass(commandBuffer);
				tveRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(tveDevice.device());
	}

	void FirstApp::loadGameObjects() {
		std::shared_ptr<TveModel> tveModel = TveModel::
			createModelFromFile(tveDevice, "models/smooth_vase.obj");

        auto smoothVase = TveGameObject::createGameObject();
        smoothVase.model = tveModel;
        //obj.transform.translation = { .0f, .0f, 2.5f };
		smoothVase.transform.translation = { -.5f, .5f, 0.f };
        //obj.transform.scale = glm::vec3(2.f);
		smoothVase.transform.scale = {3.f, 1.5f, 3.f};
		gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		
		tveModel = TveModel::createModelFromFile(tveDevice, "models/flat_vase.obj");

		auto flatVase = TveGameObject::createGameObject();
		flatVase.model = tveModel;
		flatVase.transform.translation = { .5f, .5f, 0.f };
		//obj.transform.scale = glm::vec3(2.f);
		flatVase.transform.scale = { 3.f, 1.5f, 3.f };
		gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		tveModel = TveModel::createModelFromFile(tveDevice, "models/floor.obj");
		auto floor = TveGameObject::createGameObject();
		floor.model = tveModel;
		//obj.transform.translation = { .0f, .0f, 2.5f };
		floor.transform.translation = { 0.f, .5f, 0.f };
		//obj.transform.scale = glm::vec3(2.f);
		floor.transform.scale = { 3.f, 1.f, 3.f };
		gameObjects.emplace(floor.getId(), std::move(floor));
	}

} // namespace tve