#pragma once
#include <vulkan/vulkan.hpp>
namespace VulkanUtils
{
	void beginJob(vk::CommandBuffer cmdBuffer);
	void endJob(vk::CommandBuffer cmdBuffer, vk::Queue queue);
};

