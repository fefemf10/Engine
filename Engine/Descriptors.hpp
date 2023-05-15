#pragma once
#include <vulkan/vulkan.hpp>
namespace Vulkan
{
	struct DescriptorSetLayoutData
	{
		int count;
		std::vector<int> indices;
		std::vector<vk::DescriptorType> types;
		std::vector<int> counts;
		std::vector<vk::ShaderStageFlags> stages;
	};
	vk::DescriptorSetLayout createDescriptorSetLayout(vk::Device device, const DescriptorSetLayoutData& bindings);
	vk::DescriptorPool createDescriptorPool(vk::Device device, uint32_t size, const DescriptorSetLayoutData& bindings);
	vk::DescriptorSet allocateDescriptorSet(vk::Device device, vk::DescriptorPool descriptorPool, vk::DescriptorSetLayout descriptorSetLayout);
};

