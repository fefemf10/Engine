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
		bundle.frames[i].imageView = VulkanImage::createImageView(device, images[i], format.format, vk::ImageAspectFlagBits::eColor);
	}
	bundle.format = format.format;
	bundle.extent = extent2D;

	return bundle;
}

void Vulkan::SwapchainFrame::createDescriptorResources()
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

void Vulkan::SwapchainFrame::createDepthResources()
{
	depthFormat = VulkanImage::getSupportedFormat(physicalDevice, { vk::Format::eD32Sfloat, vk::Format::eD24UnormS8Uint }, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
	VulkanImage::ImageInputChunk imageInfo;
	imageInfo.physicalDevice = physicalDevice;
	imageInfo.device = device;
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
	imageInfo.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
	imageInfo.width = width;
	imageInfo.height = height;
	imageInfo.format = depthFormat;
	depthBuffer = VulkanImage::createImage(imageInfo);
	depthBufferMemory = VulkanImage::createImageMemory(imageInfo, depthBuffer);
	depthBufferView = VulkanImage::createImageView(device, depthBuffer, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

void Vulkan::SwapchainFrame::writeDescriptorSet()
{
	vk::WriteDescriptorSet writeInfo(descriptorSet, 0, 0, vk::DescriptorType::eUniformBuffer, {}, uniformBufferDescriptor);
	vk::WriteDescriptorSet writeInfo2(descriptorSet, 1, 0, vk::DescriptorType::eStorageBuffer, {}, modelBufferDescriptor);
	device.updateDescriptorSets({ writeInfo, writeInfo2 }, nullptr);
}

void Vulkan::SwapchainFrame::destroy()
{
	device.destroyImage(depthBuffer);
	device.freeMemory(depthBufferMemory);
	device.destroyImageView(depthBufferView);

	device.destroyImageView(imageView);
	device.destroyFramebuffer(framebuffer);
	device.destroyFence(inFlight);
	device.destroySemaphore(imageAvailable);
	device.destroySemaphore(renderFinished);

	device.unmapMemory(cameraDataBuffer.bufferMemory);
	device.freeMemory(cameraDataBuffer.bufferMemory);
	device.destroyBuffer(cameraDataBuffer.buffer);

	device.unmapMemory(modelBuffer.bufferMemory);
	device.freeMemory(modelBuffer.bufferMemory);
	device.destroyBuffer(modelBuffer.buffer);
}
