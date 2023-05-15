#pragma once
#include "Swapchain.hpp"

namespace Vulkan
{
	struct FramebufferInput
	{
		vk::Device device;
		vk::RenderPass renderPass;
		vk::Extent2D swapchainExtent;
	};

	void createFramebuffers(FramebufferInput& inputChunk, std::vector<SwapchainFrame>& frames);
}