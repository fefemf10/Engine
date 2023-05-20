#include "Framebuffer.hpp"
#include "Log.hpp"

void Vulkan::createFramebuffers(FramebufferInput& inputChunk, std::vector<SwapchainFrame>& frames)
{
	for (size_t i = 0; i < frames.size(); i++)
	{
		std::vector<vk::ImageView> attachments = { frames[i].imageView, frames[i].depthBufferView};

		vk::FramebufferCreateInfo framebufferInfo({}, inputChunk.renderPass, attachments, inputChunk.swapchainExtent.width, inputChunk.swapchainExtent.height, 1);
		Log::debug("{}\n", inputChunk.device.createFramebuffer(&framebufferInfo, nullptr, &frames[i].framebuffer) == vk::Result::eSuccess ? "Successfully created framebuffer" : "Failed to create framebuffer");
	}
}