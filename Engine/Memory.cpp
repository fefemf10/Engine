#include "Memory.hpp"
#include "SingleTimeJob.hpp"
#include "Log.hpp"

VulkanUtils::Buffer VulkanUtils::createBuffer(BufferInput& input)
{
    vk::BufferCreateInfo bufferInfo({}, input.size, input.usage, vk::SharingMode::eExclusive);
    Buffer buffer;
    Log::debug("{}\n", input.device.createBuffer(&bufferInfo, nullptr, &buffer.buffer) == vk::Result::eSuccess ? "Successfully created buffer" : "Failed to create buffer");
    allocateBufferMemory(buffer, input);
    return buffer;
}

void VulkanUtils::copyBuffer(Buffer& scrBuffer, Buffer& dstBuffer, vk::DeviceSize size, vk::Queue queue, vk::CommandBuffer cmdBuffer)
{
    beginJob(cmdBuffer);
    vk::BufferCopy copyRegion(0, 0, size);
    cmdBuffer.copyBuffer(scrBuffer.buffer, dstBuffer.buffer, copyRegion);
    endJob(cmdBuffer, queue);
}

uint32_t VulkanUtils::findMemoryTypeIndex(const vk::PhysicalDevice& physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties)
{
    const vk::PhysicalDeviceMemoryProperties& memoryProperties = physicalDevice.getMemoryProperties();
    for (size_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
    {
        bool supported{ bool(supportedMemoryIndices & (1 << i)) };
        bool sufficient{ (memoryProperties.memoryTypes[i].propertyFlags & requestedProperties) == requestedProperties };
        if (supported && sufficient)
            return i;
    }
    return 0;
}

void VulkanUtils::allocateBufferMemory(Buffer& buffer, const BufferInput& input)
{
    vk::MemoryRequirements memoryRequirements = input.device.getBufferMemoryRequirements(buffer.buffer);
    vk::MemoryAllocateInfo allocInfo(memoryRequirements.size, findMemoryTypeIndex(input.physicalDevice, memoryRequirements.memoryTypeBits, input.memoryProperties));
    Log::debug("{}\n", input.device.allocateMemory(&allocInfo, nullptr, &buffer.bufferMemory) == vk::Result::eSuccess ? "Successfully allocated buffer" : "Failed to allocate buffer");
    input.device.bindBufferMemory(buffer.buffer, buffer.bufferMemory, 0);
}
