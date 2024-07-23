﻿/*
	Main class for the Vulkan engine.
*/
#pragma once

#include <tv_types.h>

#include "tv_descriptors.h"
#include "tv_loader.h"
#include "tv_camera.h"

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
	// Global data descriptor for every frame
	DescriptorAllocator _frameDescriptors;
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

// Compute shaders
struct ComputeEffect {
	const char* name;

	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;

	ComputePushConstants pushConstants;
};

struct RenderObject {
	uint32_t indexCount;
	uint32_t firstIndex;
	VkBuffer indexBuffer;

	MaterialInstance* material;
	Bounds bounds;
	glm::mat4 transform;
	VkDeviceAddress vertexBufferAddress;
};

struct DrawContext {
	std::vector<RenderObject> OpaqueSurfaces;
	std::vector<RenderObject> TransparentSurfaces;
};

struct MeshNode : public Node {

	std::shared_ptr<MeshAsset> mesh;

	virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
};

struct GLTFMetallic_Roughness {
	MaterialPipeline opaquePipeline;
	MaterialPipeline transparentPipeline;

	VkDescriptorSetLayout materialLayout;

	struct MaterialConstants {
		glm::vec4 colorFactors;
		glm::vec4 metal_rough_factors;
		//padding, we need it anyway for uniform buffers
		glm::vec4 extra[14];
	};

	struct MaterialResources {
		AllocatedImage colorImage;
		VkSampler colorSampler;
		AllocatedImage metalRoughImage;
		VkSampler metalRoughSampler;
		VkBuffer dataBuffer;
		uint32_t dataBufferOffset;
	};

	DescriptorWriter writer;

	/*
		Initializes the opaque and transparent pipelines.
	*/
	void build_pipelines(TinyVulkan* engine);
	/*
		Destroys the opaque and transparent pipelines.
	*/
	void clear_resources(VkDevice device);
	/*
		Selects opaque or transparent pipeline, allocates descriptor set, and writes it using
		the image and buffer from MaterialResources.
	*/
	MaterialInstance write_material(VkDevice device, MaterialPass pass, const MaterialResources& resources, DescriptorAllocator& descriptorAllocator);
};

struct EngineStats {
	float frametime;
	int triangle_count;
	int drawcall_count;
	float scene_update_time;
	float mesh_draw_time;
};

class TinyVulkan {
public:
	// Main deletion queue for global objects
	DeletionQueue _mainDeletionQueue;

	// VMA memory allocator object
	VmaAllocator _allocator;

	// Camera object
	Camera mainCamera;

	// Engine stats
	EngineStats stats;

	// Creates a buffer
	AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usageFlags, VmaMemoryUsage memoryUsage);
	// Destroys a buffer
	void destroy_buffer(const AllocatedBuffer& buffer);

	// Draw resources
	AllocatedImage _drawImage;
	AllocatedImage _depthImage;
	VkExtent2D _drawExtent;
	float renderScale = 1.f;

	AllocatedImage create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
	AllocatedImage create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
	void destroy_image(const AllocatedImage& img);

	// Default textures
	AllocatedImage _whiteImage;
	AllocatedImage _blackImage;
	AllocatedImage _greyImage;
	AllocatedImage _errorCheckerboardImage;

	VkSampler _defaultSamplerLinear;
	VkSampler _defaultSamplerNearest;

	MaterialInstance defaultData;
	GLTFMetallic_Roughness metalRoughMaterial;
	

	bool _isInitialized{ false };
	int _frameNumber {0};
	bool stop_rendering{ false };
	bool resize_requested{ false };
	VkExtent2D _windowExtent{ 1700 , 900 };

	struct SDL_Window* _window{ nullptr }; //forward declaration

	static TinyVulkan& Get();

	// Initializes everything in the engine, creates SDL Window, and loads scenes
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

	VkPipelineLayout _gradientPipelineLayout;

	// immediate submit structures
	VkFence _immFence;
	VkCommandBuffer _immCommandBuffer;
	VkCommandPool _immCommandPool;

	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

	// Stores an array of compute pipelines
	std::vector<ComputeEffect> backfroundEffects;
	int currentBackgroundEffect{ 0 };

	// Creates mesh buffers and uploads them to the GPU
	GPUMeshBuffers uploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);

	// Unfiform buffer of scene data
	GPUSceneData sceneData;

	VkDescriptorSetLayout _gpuSceneDataDescriptorLayout;

	DrawContext mainDrawContext;
	std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> loadedScenes;
	bool bShouldRenderStructure = false;
	bool bShouldRenderSponza = false;
	

	void update_scene();
private:
	/*
		Initializes Vulkan instance, creates the logical VkDevice, and initializes the
		VMA memory allocator.
	*/
	void init_vulkan();
	/*
		Initializes swapchain and creates the draw and depth images.
	*/
	void init_swapchain();
	/*
		Creates a swapchain using VKBoostrapper.
	*/
	void create_swapchain(uint32_t width, uint32_t height);
	/*
		Destroys and rebuilds the swapchain when the window is resized.
	*/
	void resize_swapchain();
	/*
		Destroys swapchain object and its ImageView resources.
	*/
	void destroy_swapchain();
	/*
		Initializes command buffers for submitting commands to the graphics queue.
	*/
	void init_commands();
	/*
		Initializes fences and semaphores for synchronization. 
	*/
	void init_sync_structures();
	/*
		Initializes descriptor sets for binding to the shaders.
	*/
	void init_descriptors();
	/*
		Main function for initializing pipelines.
	*/
	void init_pipelines();
	/*
		Initializes the compute pipeline to generate a background effect.
	*/
	void init_compute_pipelines();
	/*
		Initializes the ImGUI library and its Vulkan-related parameters.
	*/
	void init_imgui();
	/*
		Initializes default values for textures and materials.
	*/
	void init_default_data();
};