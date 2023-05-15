#pragma once
#include <vulkan/vulkan.hpp>

namespace Vulkan
{
	vk::Semaphore createSemaphore(vk::Device& device);
	vk::Fence createFence(vk::Device& device);
}