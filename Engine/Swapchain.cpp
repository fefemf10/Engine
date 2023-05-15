#include "Swapchain.hpp"
#include "Utils.hpp"
#include "Log.hpp"
#include "Image.hpp"

vk::SurfaceFormatKHR Vulkan::findSurfaceFormat(std::span<vk::SurfaceFormatKHR> formats)
{
	for (const vk::SurfaceFormatKHR& format : formats)
		if (format.format == vk::Format::eR8G8B8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			return format;
	return formats[0];
}
vk::PresentModeKHR Vulkan::findPresentMode(std::span<vk::PresentModeKHR> presentModes)
{
	for (const vk::PresentModeKHR& present : presentModes)
		if (present == vk::PresentModeKHR::eMailbox)
			return present;
	return vk::PresentModeKHR::eFifo;
}
vk::Extent2D Vulkan::findExtent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
		return capabilities.currentExtent;
	else
	{
		vk::Extent2D extent = { width, height };
		extent.width = std::min(capabilities.maxImageExtent.width, std::max(capabilities.minImageExtent.width, width));
		extent.height = std::min(capabilities.maxImageExtent.height, std::max(capabilities.minImageExtent.height, height));
		return extent;
	}
}
Vulkan::SwapchainBundle Vulkan::createSwapchainBundle(vk::PhysicalDevice& physicalDevice, vk::Device& device, vk::SurfaceKHR& surface, uint32_t width, uint32_t height)
{
	SwapchainSupportDetails swapchainSupportDetails;
	swapchainSupportDetails.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	swapchainSupportDetails.formats = physicalDevice.getSurfaceFormatsKHR(surface);
	swapchainSupportDetails.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

	vk::SurfaceFormatKHR format = findSurfaceFormat(swapchainSupportDetails.formats);
	vk::PresentModeKHR presentMode = findPresentMode(swapchainSupportDetails.presentModes);
	vk::Extent2D extent2D = findExtent(width, height, swapchainSupportDetails.capabilities);
	uint32_t imageCount = std::min(swapchainSupportDetails.capabilities.maxImageCount, swapchainSupportDetails.capabilities.minImageCount + 1);

	vk::SwapchainCreateInfoKHR swapchainCreateInfo(vk::SwapchainCreateFlagsKHR(), surface, imageCount, format.format, format.colorSpace, extent2D, 1, vk::ImageUsageFlagBits::eColorAttachment);
	VulkanUtils::QueueFamilyIndices indices = VulkanUtils::findQueueFamilies(physicalDevice, surface);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (queueFamilyIndices[0] != queueFamilyIndices[1])
	{
		swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
	}

	swapchainCreateInfo.preTransform = swapchainSupportDetails.capabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	swapchainCreateInfo.presentMode = presentMode;
	swapchainCreateInfo.clipped = VK_TRUE;

	swapchainCreateInfo.oldSwapchain = vk::SwapchainKHR();

	SwapchainBundle bundle{};
	Log::debug("{}\n", device.createSwapchainKHR(&swapchainCreateInfo, nullptr, &bundle.swapchain) == vk::Result::eSuccess ? "Successfully created swapchain" : "Failed to create swapchain");
	const std::vector<vk::Image>& images = device.getSwapchainImagesKHR(bundle.swapchain);
	bundle.frames.resize(images.size());
	for (size_t i = 0; i < images.size(); ++i)
	{
		bundle.frames[i].image = images[i];
		bundle.frames[i].imageView = VulkanImage::createImageView(device, images[i], format.format);
	}
	bundle.format = format.format;
	bundle.extent = extent2D;

	return bundle;
	/*findSwapchainImages();

	vk::CommandPoolCreateInfo cmdPoolCreateInfo({}, graphicsQueue);
	if (device.createCommandPool(&cmdPoolCreateInfo, nullptr, &queue.cmdPool) != vk::Result::eSuccess)
		throw std::runtime_error("An error in the vkCreateCommandPool function");

	cmdBuffers.resize(swapchainImages.size());

	vk::CommandBufferAllocateInfo cmdBufferAllocateInfo(queue.cmdPool, vk::CommandBufferLevel::ePrimary, cmdBuffers.size());
	if (device.allocateCommandBuffers(&cmdBufferAllocateInfo, cmdBuffers.data()) != vk::Result::eSuccess)
		throw std::runtime_error("An error in the vkAllocateCommandBuffers function");

	vk::CommandBufferBeginInfo cmdBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
	clearColorValue = std::array<float, 4>{ 1.f, 0.5f, 0.f, 0.f };
	vk::ImageSubresourceRange imageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

	for (size_t i = 0; i < swapchainImages.size(); i++)
	{
		vk::ImageMemoryBarrier imageMemoryBarrierPresentToClear(vk::AccessFlagBits::eMemoryRead, vk::AccessFlagBits::eTransferWrite, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, queue.graphicsFamily.value(), queue.graphicsFamily.value(), swapchainImages[i], imageSubresourceRange);
		vk::ImageMemoryBarrier imageMemoryBarrierClearToPresent(vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eMemoryRead, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR, queue.graphicsFamily.value(), queue.graphicsFamily.value(), swapchainImages[i], imageSubresourceRange);
		cmdBuffers[i].begin(&cmdBufferBeginInfo);
		cmdBuffers[i].pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrierPresentToClear);
		cmdBuffers[i].clearColorImage(swapchainImages[i], vk::ImageLayout::eTransferDstOptimal, &clearColorValue, 1, &imageSubresourceRange);
		cmdBuffers[i].pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrierClearToPresent);
		cmdBuffers[i].end();
	}

	vk::SemaphoreCreateInfo semaphoreCreateInfo;
	if ((device.createSemaphore(&semaphoreCreateInfo, nullptr, &semaphoreImageAvailable) != vk::Result::eSuccess) ||
		(device.createSemaphore(&semaphoreCreateInfo, nullptr, &semaphoreRenderingFinished) != vk::Result::eSuccess))
		throw std::runtime_error("An error in the vk::CreateSemaphore function"); */
}

void Vulkan::SwapchainFrame::createDescriptorResources(vk::PhysicalDevice physicalDevice, vk::Device device)
{
	VulkanUtils::BufferInput input;
	input.physicalDevice = physicalDevice;
	input.device = device;
	input.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	input.size = sizeof(MVP);
	input.usage = vk::BufferUsageFlagBits::eUniformBuffer;
	cameraDataBuffer = VulkanUtils::createBuffer(input);
	
	cameraDataWriteLocation = device.mapMemory(cameraDataBuffer.bufferMemory, 0, sizeof(MVP));
	
	input.size = 1024 * sizeof(glm::mat4);
	input.usage = vk::BufferUsageFlagBits::eStorageBuffer;
	modelBuffer = VulkanUtils::createBuffer(input);

	modelBufferWriteLocation = device.mapMemory(modelBuffer.bufferMemory, 0, 1024 * sizeof(glm::mat4));

	modelTransform.reserve(1024);

	for (size_t i = 0; i < 1024; i++)
	{
		modelTransform.push_back(glm::mat4(1.f));
	}

	uniformBufferDescriptor.buffer = cameraDataBuffer.buffer;
	uniformBufferDescriptor.offset = 0;
	uniformBufferDescriptor.range = sizeof(MVP);

	modelBufferDescriptor.buffer = modelBuffer.buffer;
	modelBufferDescriptor.offset = 0;
	modelBufferDescriptor.range = 1024 * sizeof(glm::mat4);
}

void Vulkan::SwapchainFrame::writeDescriptorSet(vk::Device device)
{
	vk::WriteDescriptorSet writeInfo(descriptorSet, 0, 0, vk::DescriptorType::eUniformBuffer, {}, uniformBufferDescriptor);
	vk::WriteDescriptorSet writeInfo2(descriptorSet, 1, 0, vk::DescriptorType::eStorageBuffer, {}, modelBufferDescriptor);
	device.updateDescriptorSets({ writeInfo, writeInfo2 }, nullptr);
}
