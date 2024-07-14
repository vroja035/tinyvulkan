/*
	Descriptor set abstractions.
*/
#pragma once

#include <vk_types.h>

// Abstraction to build a DescriptorSet
struct DescriptorLayoutBuilder 
{

    std::vector<VkDescriptorSetLayoutBinding> bindings;

    // Add a DescriptorSetLayoutBinding
    void add_binding(uint32_t binding, VkDescriptorType type);
    // Clear all bindings
    void clear();
    // Create the VkDescriptorSetLayout
    VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
};

// Abstraction to allocate a VkDescriptorSet from a VkDescriptorPool
struct DescriptorAllocator {
public:
    // Size of the VkDescriptorPool
    struct PoolSizeRatio {
        VkDescriptorType type;
        float ratio;
    };

    // Creates the VkDescriptorPool according to the PoolSizeRatio
    void init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
    // Does not delete the VkDescriptorPool, only the sets and resets the pool
    void clear_descriptors(VkDevice device);
    // Destroys the VkDescriptorPool
    void destroy_pools(VkDevice device);
    // Allocates VkDescriptorSet(s) from the pool
    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout, void* pNext = nullptr);
private:
    // Gets pool from readyPools, otherwise creates a new one
    VkDescriptorPool get_pool(VkDevice device);
    VkDescriptorPool create_pool(VkDevice device, uint32_t setCount, std::span<PoolSizeRatio> poolRatios);

    std::vector<PoolSizeRatio> ratios;
    // Unusable pools
    std::vector<VkDescriptorPool> fullPools;
    // Usable pools
    std::vector<VkDescriptorPool> readyPools;
    uint32_t setsPerPool;

};

// Abstraction for writing descriptor set objects
struct DescriptorWriter {
    std::deque<VkDescriptorImageInfo> imageInfos;
    std::deque<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkWriteDescriptorSet> writes;

    // Writes image resources
    void write_image(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type);
    // Writes buffer resources
    void write_buffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type);
    // Resets everything
    void clear();
    // Updates the contents of a descriptor set  
    void update_set(VkDevice device, VkDescriptorSet set);
};