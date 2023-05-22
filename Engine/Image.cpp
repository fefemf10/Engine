#include "Image.hpp"
#include "Memory.hpp"
#include "Descriptors.hpp"
#include "SingleTimeJob.hpp"
#include "Log.hpp"
#include "stb_image.hpp"

VulkanImage::Texture::Texture(TextureInputChunk& input)
{
	physicalDevice = input.physicalDevice;
	device = input.device;
	filename = input.filename;
	cmdBuffer = input.cmdBuffer;
	queue = input.queue;
	descriptorlayout = input.descriptorLayout;
	descriptorPool = input.descriptorPool;
	load();
	ImageInputChunk imageInput;
	imageInput.physicalDevice = physicalDevice;
	imageInput.device = device;
	imageInput.width = width;
	imageInput.height = height;
	imageInput.format = vk::Format::eR8G8B8A8Unorm;
	imageInput.tiling = vk::ImageTiling::eOptimal;
	imageInput.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
	imageInput.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
	image = createImage(imageInput);
	imageMemory = createImageMemory(imageInput, image);
	populate();
	free(pixels);
	createView();
	createSampler();
	createDescriptorSet();
}

VulkanImage::Texture::~Texture()
{
	device.free(imageMemory);
	device.destroyImage(image);
	device.destroyImageView(imageView);
	device.destroySampler(sampler);
}

void VulkanImage::Texture::use(vk::CommandBuffer cmdBuffer, vk::PipelineLayout layout)
{
	cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 1, descriptorSet, nullptr);
}

void VulkanImage::Texture::load()
{
	pixels = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
	if (!pixels)
		Log::debug("Failed to load: {}", filename);

}

void VulkanImage::Texture::populate()
{
	VulkanUtils::BufferInput input;
	input.physicalDevice = physicalDevice;
	input.device = device;
	input.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	input.usage = vk::BufferUsageFlagBits::eTransferSrc;
	input.size = width * height * 4;
	VulkanUtils::Buffer stagingBuffer = VulkanUtils::createBuffer(input);

	void* writeLocation = device.mapMemory(stagingBuffer.bufferMemory, 0, input.size);
	memcpy(writeLocation, pixels, input.size);
	device.unmapMemory(stagingBuffer.bufferMemory);

	ImageLayoutTransitionJob transitionJob;
	transitionJob.cmdBuffer = cmdBuffer;
	transitionJob.queue = queue;
	transitionJob.image = image;
	transitionJob.oldLayout = vk::ImageLayout::eUndefined;
	transitionJob.newLayout = vk::ImageLayout::eTransferDstOptimal;
	transitionImageLayout(transitionJob);

	BufferImageCopyJob copyJob;
	copyJob.cmdBuffer = cmdBuffer;
	copyJob.queue = queue;
	copyJob.srcBuffer = stagingBuffer.buffer;
	copyJob.dstImage = image;
	copyJob.width = width;
	copyJob.height = height;
	copyBufferToImage(copyJob);

	transitionJob.oldLayout = vk::ImageLayout::eTransferDstOptimal;
	transitionJob.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	transitionImageLayout(transitionJob);

	device.freeMemory(stagingBuffer.bufferMemory);
	device.destroyBuffer(stagingBuffer.buffer);
}

void VulkanImage::Texture::createView()
{
	imageView = createImageView(device, image, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);
}

void VulkanImage::Texture::createSampler()
{
	vk::SamplerCreateInfo samplerInfo({}, vk::Filter::eNearest, vk::Filter::eLinear,
		vk::SamplerMipmapMode::eNearest,
		vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat,
		0.f, false, 1.f, false, vk::CompareOp::eAlways, 0.f, 0.f, vk::BorderColor::eIntOpaqueBlack, false);
	Log::debug("{}\n", device.createSampler(&samplerInfo, nullptr, &sampler) == vk::Result::eSuccess ? "Successfully created sampler" : "Failed to create sampler");
}

void VulkanImage::Texture::createDescriptorSet()
{
	descriptorSet = Vulkan::allocateDescriptorSet(device, descriptorPool, descriptorlayout);

	vk::DescriptorImageInfo imageDescriptor(sampler, imageView, vk::ImageLayout::eShaderReadOnlyOptimal);

	vk::WriteDescriptorSet descriptorWrite(descriptorSet, 0, 0, vk::DescriptorType::eCombinedImageSampler, imageDescriptor);
	device.updateDescriptorSets(descriptorWrite, nullptr);
}

vk::Image VulkanImage::createImage(ImageInputChunk input)
{
	vk::Image image;
	vk::ImageCreateInfo imageInfo({}, vk::ImageType::e2D, input.format, vk::Extent3D(input.width, input.height, 1), 1, 1, vk::SampleCountFlagBits::e1, input.tiling, input.usage, vk::SharingMode::eExclusive, {}, vk::ImageLayout::eUndefined);
	Log::debug("{}\n", input.device.createImage(&imageInfo, nullptr, &image) == vk::Result::eSuccess ? "Successfully created image" : "Failed to create image");
	return image;
}

vk::DeviceMemory VulkanImage::createImageMemory(ImageInputChunk input, vk::Image image)
{
	vk::MemoryRequirements requirements = input.device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocation;
	allocation.allocationSize = requirements.size;
	allocation.memoryTypeIndex = VulkanUtils::findMemoryTypeIndex(input.physicalDevice, requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
	vk::DeviceMemory deviceMemory;
	Log::debug("{}\n", input.device.allocateMemory(&allocation, nullptr, &deviceMemory) == vk::Result::eSuccess ? "Successfully allocated memory" : "Failed to allocate memory");
	input.device.bindImageMemory(image, deviceMemory, 0);
	return deviceMemory;
}

void VulkanImage::transitionImageLayout(ImageLayoutTransitionJob job)
{
	VulkanUtils::beginJob(job.cmdBuffer);
	vk::ImageSubresourceRange access(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
	vk::ImageMemoryBarrier barrier({}, {}, job.oldLayout, job.newLayout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, job.image, access);
	vk::PipelineStageFlags sourceStage, dstStage;
	if (job.oldLayout == vk::ImageLayout::eUndefined && job.newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		dstStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		dstStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	job.cmdBuffer.pipelineBarrier(sourceStage, dstStage, vk::DependencyFlags(), nullptr, nullptr, barrier);
	VulkanUtils::endJob(job.cmdBuffer, job.queue);
}

void VulkanImage::copyBufferToImage(BufferImageCopyJob job)
{
	VulkanUtils::beginJob(job.cmdBuffer);
	vk::BufferImageCopy copy(0, 0, 0);
	vk::ImageSubresourceLayers access(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
	copy.imageSubresource = access;

	copy.imageOffset = vk::Offset3D(0, 0, 0);
	copy.imageExtent = vk::Extent3D(job.width, job.height, 1);
	job.cmdBuffer.copyBufferToImage(job.srcBuffer, job.dstImage, vk::ImageLayout::eTransferDstOptimal, copy);
	VulkanUtils::endJob(job.cmdBuffer, job.queue);
}

vk::ImageView VulkanImage::createImageView(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspect)
{
	vk::ImageViewCreateInfo createInfo({}, image, vk::ImageViewType::e2D, format, { vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity });
	createInfo.subresourceRange.aspectMask = aspect;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	return device.createImageView(createInfo);
}

vk::Format VulkanImage::getSupportedFormat(vk::PhysicalDevice physicalDevice, std::vector<vk::Format> candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
	for (vk::Format format : candidates)
	{
		vk::FormatProperties properties = physicalDevice.getFormatProperties(format);
		if (tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features)
		{
			return format;
		}
		if (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}
	std::runtime_error("Unable to find suitable format");
}

