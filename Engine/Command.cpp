#include "Command.hpp"
#include "Log.hpp"

vk::CommandPool Vulkan::createCmdPool(vk::PhysicalDevice& physicalDevice, vk::Device& device, vk::SurfaceKHR& surface)
{
	VulkanUtils::QueueFamilyIndices indices = VulkanUtils::findQueueFamilies(physicalDevice, surface);
	
	vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, indices.graphicsFamily.value());
	vk::CommandPool cmdPool;
	Log::debug("{}\n", device.createCommandPool(&poolInfo, nullptr, &cmdPool) == vk::Result::eSuccess ? "Successfully created command pool" : "Failed to create command pool");
	return cmdPool;
}

vk::CommandBuffer Vulkan::createCmdBuffer(cmdBufferInputChunk& inputChunk)
{
	vk::CommandBufferAllocateInfo allocInfo(inputChunk.cmdPool, vk::CommandBufferLevel::ePrimary, 1);
	vk::CommandBuffer cmdBuffer;
	Log::debug("{}\n", inputChunk.device.allocateCommandBuffers(&allocInfo, &cmdBuffer) == vk::Result::eSuccess ? "Successfully allocated main command buffer" : "Failed to allocate main command buffer");
	return cmdBuffer;
}

void Vulkan::createFrameCmdBuffer(cmdBufferInputChunk& inputChunk)
{
	vk::CommandBufferAllocateInfo allocInfo(inputChunk.cmdPool, vk::CommandBufferLevel::ePrimary, 1);
	for (size_t i = 0; i < inputChunk.frames.size(); ++i)
	{
		Log::debug("{} for frame {}\n", inputChunk.device.allocateCommandBuffers(&allocInfo, &inputChunk.frames[i].cmdBuffer) == vk::Result::eSuccess ? "Successfully allocated command buffer" : "Failed to allocate command buffer", i);
	}
}

