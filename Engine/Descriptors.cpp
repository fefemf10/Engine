#include "Descriptors.hpp"
#include "Log.hpp"

vk::DescriptorSetLayout Vulkan::createDescriptorSetLayout(vk::Device device, const DescriptorSetLayoutData& bindings)
{
    std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
    layoutBindings.reserve(bindings.count);
    for (size_t i = 0; i < bindings.count; i++)
    {
        vk::DescriptorSetLayoutBinding layoutBinidng;
        layoutBinidng.binding = bindings.indices[i];
        layoutBinidng.descriptorType = bindings.types[i];
        layoutBinidng.descriptorCount = bindings.counts[i];
        layoutBinidng.stageFlags = bindings.stages[i];
        layoutBindings.push_back(layoutBinidng);
    }
    vk::DescriptorSetLayoutCreateInfo layoutInfo({}, layoutBindings);
    vk::DescriptorSetLayout descriptorSetLayout;
    Log::debug("{}\n", device.createDescriptorSetLayout(&layoutInfo, nullptr, &descriptorSetLayout) == vk::Result::eSuccess ? "Successfully created descriptor set layout" : "Failed to create descriptor set layout");
    return descriptorSetLayout;
}

vk::DescriptorPool Vulkan::createDescriptorPool(vk::Device device, uint32_t size, const DescriptorSetLayoutData& bindings)
{
    std::vector<vk::DescriptorPoolSize> poolSizes;
    for (size_t i = 0; i < bindings.count; i++)
    {
        vk::DescriptorPoolSize poolSize(bindings.types[i], size);
        poolSizes.push_back(poolSize);
    }
    vk::DescriptorPoolCreateInfo descriptorInfo({}, size, poolSizes);
    vk::DescriptorPool descriptorPool;
    Log::debug("{}\n", device.createDescriptorPool(&descriptorInfo, nullptr, &descriptorPool) == vk::Result::eSuccess ? "Successfully created descriptor pool" : "Failed to create descriptor pool");
    return descriptorPool;
}

vk::DescriptorSet Vulkan::allocateDescriptorSet(vk::Device device, vk::DescriptorPool descriptorPool, vk::DescriptorSetLayout descriptorSetLayout)
{
    vk::DescriptorSetAllocateInfo allocInfo(descriptorPool, descriptorSetLayout);
    vk::DescriptorSet descriptorSet;
    Log::debug("{}\n", device.allocateDescriptorSets(&allocInfo, &descriptorSet) == vk::Result::eSuccess ? "Successfully allocated descriptor set from pool" : "Failed to allocate descriptor set from pool");
    return descriptorSet;
}
