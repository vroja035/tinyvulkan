/*
	
	Cannot directly bind an uniform buffer to be used by a pipeline object.
	Needs a descriptor set.

	Descriptor points to a resource and store additional data. Cannot be bound individually, only in a set.
	Should be grouped into sets based on binding freq.

	DescriptorSetLayouts -> Create pipeline layout
	- providing pipeline with a blueprint of how many descriptor sets to expect and their attributes
	- pipeleine will expect these sets to be bound

	Cannot create descriptor sets directly; needs to be allocated from a descriptor pool object.

	Summary:
	1. Descriptors point to a resource
	2. Group descriptors into sets to bind
	3. DescriptorSetLayouts needs to be provided at pipeline creation
	4. Bind descriptor sets before draw call
	5. Descriptor sets can only be created using a descriptor pool object.

*/

#pragma once

#include "tve_device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace tve {

	class TveDescriptorSetLayout {
	public:
		class Builder {
		public:
			// builds descriptor set layouts; makes blueprint obj
			Builder(TveDevice& tveDevice) : tveDevice{ tveDevice } {}

			// appends to map of binding the info vulkan requires
			Builder& addBinding(
				uint32_t binding,
				VkDescriptorType descriptorType,
				VkShaderStageFlags stageFlags,
				uint32_t count = 1);
			
			// creates instance
			std::unique_ptr<TveDescriptorSetLayout> build() const;

		private:
			TveDevice& tveDevice;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
		};

		TveDescriptorSetLayout(
			TveDevice& tveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~TveDescriptorSetLayout();
		TveDescriptorSetLayout(const TveDescriptorSetLayout&) = delete;
		TveDescriptorSetLayout& operator=(const TveDescriptorSetLayout&) = delete;

		VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

	private:
		TveDevice& tveDevice;
		VkDescriptorSetLayout descriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

		friend class TveDescriptorWriter;
	};

	class TveDescriptorPool {
	public:
		class Builder {
		public:
			Builder(TveDevice& tveDevice) : tveDevice{ tveDevice } {}

			// how many descriptors of each type to expect
			Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
			// describes the behaviour of the pool obj
			Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
			// total # of descriptor sets
			Builder& setMaxSets(uint32_t count);
			std::unique_ptr<TveDescriptorPool> build() const;

		private:
			TveDevice& tveDevice;
			std::vector<VkDescriptorPoolSize> poolSizes{};
			uint32_t maxSets = 1000;
			VkDescriptorPoolCreateFlags poolFlags = 0;
		};

		TveDescriptorPool(
			TveDevice& tveDevice,
			uint32_t maxSets,
			VkDescriptorPoolCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes);
		~TveDescriptorPool();
		TveDescriptorPool(const TveDescriptorPool&) = delete;
		TveDescriptorPool& operator=(const TveDescriptorPool&) = delete;

		bool allocateDescriptorSet(
			const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

		void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

		void resetPool();

	private:
		TveDevice& tveDevice;
		VkDescriptorPool descriptorPool;

		friend class TveDescriptorWriter;
	};

	class TveDescriptorWriter {
	public:
		TveDescriptorWriter(TveDescriptorSetLayout& setLayout, TveDescriptorPool& pool);

		TveDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		TveDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);

	private:
		TveDescriptorSetLayout& setLayout;
		TveDescriptorPool& pool;
		std::vector<VkWriteDescriptorSet> writes;
	};


}