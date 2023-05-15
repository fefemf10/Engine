#pragma once
#include <vulkan/vulkan.hpp>

namespace VulkanUtils
{
	struct BufferInput
	{
		size_t size;
		vk::BufferUsageFlags usage;
		vk::Device device;
		vk::PhysicalDevice physicalDevice;
		vk::MemoryPropertyFlags memoryProperties;
	};

	struct Buffer
	{
		vk::Buffer buffer;
		vk::DeviceMemory bufferMemory;
	};

	Buffer createBuffer(BufferInput& input);
	void copyBuffer(Buffer& scrBuffer, Buffer& dstBuffer, vk::DeviceSize size, vk::Queue queue, vk::CommandBuffer cmdBuffer);

	uint32_t findMemoryTypeIndex(const vk::PhysicalDevice& physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties);
	void allocateBufferMemory(Buffer& buffer, const BufferInput& input);
};

