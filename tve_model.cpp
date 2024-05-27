#include "tve_model.h"

//std
#include <cassert>
#include <cstring>

namespace tve {
	TveModel::TveModel(TveDevice& device, const std::vector<Vertex>& vertices) : tveDevice(device) 
	{
	
		createVertexBuffers(vertices);

	}
	TveModel::~TveModel() {

		vkDestroyBuffer(tveDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(tveDevice.device(), vertexBufferMemory, nullptr);

	}

	void TveModel::createVertexBuffers(const std::vector<Vertex>& vertices) {

		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3!");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		tveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, //allocated mem to be accessible by cpu | keeps host and device mem regions consistent with each other
			vertexBuffer,
			vertexBufferMemory);

		void* data;
		
		// creates region of CPU memory mapped to GPU memory and sets data to point to the beginning
		// of mapped memory range
		vkMapMemory(tveDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		
		// copies vertex data into host mapped memory region
		// host memory will automatically be flushed to update the device memory 
		// due to VK_MEMORY_PROPERTY_HOST_COHERENT_BIT otherwise need to call 
		// vkFlushMappedMemoryRanges for changes to occur
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(tveDevice.device(), vertexBufferMemory);
		
	}

	void TveModel::bind(VkCommandBuffer commandBuffer) {

		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

	}
	
	void TveModel::draw(VkCommandBuffer commandBuffer) {
	
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);

	}

	std::vector<VkVertexInputBindingDescription> TveModel::Vertex::getBindingDescriptions() {

		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;

	}

	std::vector<VkVertexInputAttributeDescription> TveModel::Vertex::getAttributeDescriptions() {

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = 0;
		return attributeDescriptions;

	}

	
} // namespace tve