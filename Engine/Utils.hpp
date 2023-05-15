#pragma once
#include <vulkan/vulkan.hpp>

namespace VulkanUtils
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		vk::Queue queue;
		vk::CommandPool cmdPool;
		bool isComplete();
	};
	QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface);
}