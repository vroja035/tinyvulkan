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
struct DescriptorAllocator 
{
    // Size of the VkDescriptorPool
    struct PoolSizeRatio {
        VkDescriptorType type;
        float ratio;
    };

    VkDescriptorPool pool;

    // Creates the VkDescriptorPool according to the PoolSizeRatio
    void init_pool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
    // Does not delete the VkDescriptorPool, only the sets and resets the pool
    void clear_descriptors(VkDevice device);
    // Destroys the VkDescriptorPool
    void destroy_pool(VkDevice device);
    // Allocates VkDescriptorSet(s) from the pool
    VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
};