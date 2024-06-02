/*
	
	Veretex Shader
	---------------

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

	- Binding and Vertex Buffers do not need to be 1:1; can have 1 buffer w/ mult. bindings

	- Typically a single interleaved binding is recommended; but it depends on hardware

	Fragment Shader
	---------------

	Vertex shader runs per vertex but fragment shader occurs after rasterization and runs per
	fragment. Per vertex -> per fragment? Need to combine values throgh linear interpolation.

	Can interpolate any type of value, not just position.

	Simplest way of interpolating triangles -> Barycentric coordinates (handled by graphics pipeline
	automatically)

	For each fragment, the rasterizer will automatically interpolate any attributes passed down from
	the vertex shader into the fragment shader using barycentric coordinates. Each fragment has 
	different coordinates based on its position on the triangle.

	SIMD Model - Single Instruction Multiple Data
		- GPU able to execute same instructure on multiple input values at the same time

	Push Constant
	-------------

	Way to pass small amounts of constant data to shader via command buffer. (eg. color, offset)

	Easy and perfomant way to pass dynamic data to a shader.

	+Quick to implement
	+High performance for frequently updating data
	-Limited size 128bytes of memory for all stages

	-stage flags: specifies what shader stages will have access to it
	-offset
	-size

	Alignment requirements for objects.

	Common use: storing a transformation matrix

	---------------
	represent translation transformation in matrix form; origin must remain at 0
	cannot represent a tx,ty translation using 2x2 matrix, increase by 1, use 3x3 matrix
	fixed third row copies 1 from pos vector into transofrmed res
	2D Affine Transformation --> Homogenous Coordinate

	relative offsets/distances are affected by linear transformations
	relative offsets/distances are not affected by translations

	encoding any vector that is not a pos w/ 0 as third component

	In 3 dimentions;
	add a 4th homogenous coordinate, 4x4

	viewing volume : only what is inside the viewing volume is displayed

	Index Buffer: additional data that tells GPU how to combine vertices to make up triangles
	arr of pointers into vertex buffer

*/

#pragma once

#include "tve_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE //for vulkan
#include <glm/glm.hpp>

//std
#include <memory>
#include <vector>

namespace tve {
	
	//take vertex data from CPU, allocate memory, and copy data to GPU for rendering
	class TveModel {
	public:

		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator == (const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal &&
					uv == other.uv;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
			
			void loadModel(const std::string& filepath);
		};

		TveModel(TveDevice &device, const TveModel::Builder& builder);
		~TveModel();

		TveModel(const TveModel&) = delete;
		TveModel& operator = (const TveModel&) = delete;

		static std::unique_ptr<TveModel> createModelFromFile(
			TveDevice& device, const std::string& filepath);


		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex> &vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		TveDevice& tveDevice;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;
	};

}