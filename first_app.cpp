#include "first_app.h"

#include "keyboard_movement_controller.h"
#include "tve_camera.h"
#include "simple_render_system.h"

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

	FirstApp::FirstApp() {
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run() {
		SimpleRenderSystem simpleRenderSystem{ tveDevice, tveRenderer.getSwapChainRenderPass() };
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
				tveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				tveRenderer.endSwapChainRenderPass(commandBuffer);
				tveRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(tveDevice.device());
	}

	void FirstApp::loadGameObjects() {
		std::shared_ptr<TveModel> tveModel = TveModel::
			createModelFromFile(tveDevice, "models/colored_cube.obj");

        auto cube = TveGameObject::createGameObject();
        cube.model = tveModel;
        cube.transform.translation = { .0f, .0f, 2.5f };
        cube.transform.scale = { .5f, .5f, .5f };
        gameObjects.push_back(std::move(cube));
	}

} // namespace tve