/*
	Main class for the Vulkan engine.
*/
#pragma once

#include <vk_types.h>

#include "vk_descriptors.h"

// Handles the cleanup of objects
struct DeletionQueue
{
	// Stores callback func for every object
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush() {
		// reverse iterate the deletion queue to execute all the functions
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)(); //call functors
		}

		deletors.clear();
	}
};

// Holds structures and commands needed to draw a given frame
struct FrameData {
	// Allocator for a command buffer
	VkCommandPool _commandPool;
	// Records commands to be submitted to the device queue for execution
	VkCommandBuffer _mainCommandBuffer;
	// Used for GPU to GPU sync, link between multiple gpu queue operations
	// _swapchainSemaphore -> render commands wait on swapchain img request
	// _renderSemaphore -> waits for the draw commands of a given frame to be completed
	VkSemaphore _swapchainSemaphore, _renderSemaphore;
	// Used to sync the main loop in the CPU with the GPU
	VkFence _renderFence;
	// Deletion Queue for each frame in flight
	DeletionQueue _deletionQueue;
};
// Double-buffering
constexpr unsigned int FRAME_OVERLAP = 2;

// Compute Shaders Push Constants
struct ComputePushConstants {
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
};

struct ComputeEffect {
	const char* name;

	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;

	ComputePushConstants pushConstants;
};

class VulkanEngine {
public:
	// Main deletion queue for global objects
	DeletionQueue _mainDeletionQueue;

	// VMA memory allocator object
	VmaAllocator _allocator;

	// Draw resources
	AllocatedImage _drawImage;
	VkExtent2D _drawExtent;

	bool _isInitialized{ false };
	int _frameNumber {0};
	bool stop_rendering{ false };
	VkExtent2D _windowExtent{ 1700 , 900 };

	struct SDL_Window* _window{ nullptr }; //forward declaration

	static VulkanEngine& Get();

	// Initializes everything in the engine
	void init();

	// Destroys resources in the opposite order they were created
	void cleanup();

	// Draw loop
	void draw();
	void draw_background(VkCommandBuffer cmd);
	void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);
	void draw_geometry(VkCommandBuffer cmd);

	// Run main loop
	void run();

	VkInstance _instance;// Vulkan library handle
	VkDebugUtilsMessengerEXT _debug_messenger;// Vulkan debug output handle
	VkPhysicalDevice _chosenGPU;// GPU chosen as the default device
	VkDevice _device; // Vulkan device for commands
	VkSurfaceKHR _surface;// Vulkan window surface

	VkSwapchainKHR _swapchain;
	VkFormat _swapchainImageFormat;

	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;
	VkExtent2D _swapchainExtent;

	FrameData _frames[FRAME_OVERLAP];

	FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };

	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;

	DescriptorAllocator globalDescriptorAllocator;

	VkDescriptorSet _drawImageDescriptors;
	VkDescriptorSetLayout _drawImageDescriptorLayout;

	//VkPipeline _gradientPipeline;
	VkPipelineLayout _gradientPipelineLayout;

	// immediate submit structures
	VkFence _immFence;
	VkCommandBuffer _immCommandBuffer;
	VkCommandPool _immCommandPool;


	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

	// Stores an array of compute pipelines
	std::vector<ComputeEffect> backfroundEffects;
	int currentBackgroundEffect{ 0 };

	// Triangle pipeline
	VkPipelineLayout _trianglePipelineLayout;
	VkPipeline _trianglePipeline;
	void init_triangle_pipeline();

private:

	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();
	void init_descriptors();
	void init_pipelines();
	void init_background_pipelines();
	void init_imgui();

	void create_swapchain(uint32_t width, uint32_t height);
	void destroy_swapchain();
};
