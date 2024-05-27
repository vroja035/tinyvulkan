/*

	Vertex Buffer: chunk of memory that we pass to our vertex shader. need to tell graphics pipeline
				   how it is structured

	- Structure it by grouping it into attributes. Input variable specified per instance/per vertex 
	  basis.
		- e.g. vec2 dimensional position
		- e.g. vec3 color
	
	- Another way of structuring: grouping attributes into separate bindings.

	- Each binding needs a description
		- binding index -> vertex buffer in which it is located
		- input rate -> per vertex/instance
		- stride -> interval in bytes from one entry to next

	- Each attribute needs a Vertex Attribute Descriptions
		- binding index -> vertex buffer in which it is located
		- location value for input in vertex shader
		- offset of the attribute in bytes from the vertex start
		- format -> describes data type of attribute

	- Binding and Vertex Buffers do not need to be 1:1 can have 1 buffer w/ mult. bindings

	- Typically a single interleaved binding is recommended; but it depends on hardware

*/

#pragma once

#include "tve_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //for vulkan
#include <glm/glm.hpp>

//std
#include <vector>

namespace tve {
	
	//take vertex data from CPU, allocate memory, and copy data to GPU for rendering
	class TveModel {
	public:

		struct Vertex {
			glm::vec2 position;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		TveModel(TveDevice &device, const std::vector<Vertex>& vertices);
		~TveModel();

		TveModel(const TveModel&) = delete;
		TveModel& operator = (const TveModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex> &vertices);

		TveDevice& tveDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};

}