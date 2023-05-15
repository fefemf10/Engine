#pragma once
#include "Utils.hpp"
#include "Swapchain.hpp"

namespace Vulkan
{
	struct cmdBufferInputChunk
	{
		vk::Device device;
		vk::CommandPool cmdPool;
		std::vector<SwapchainFrame>& frames;
	};
	vk::CommandPool createCmdPool(vk::PhysicalDevice& physicalDevice, vk::Device& device, vk::SurfaceKHR& surface);
	vk::CommandBuffer createCmdBuffer(cmdBufferInputChunk& inputChunk);
	void createFrameCmdBuffer(cmdBufferInputChunk& inputChunk);
}