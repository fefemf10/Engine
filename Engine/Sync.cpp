#include "Sync.hpp"
#include "Log.hpp"

vk::Semaphore Vulkan::createSemaphore(vk::Device& device)
{
    vk::SemaphoreCreateInfo semaphoreInfo({});
    vk::Semaphore semaphore;
    Log::debug("{}\n", device.createSemaphore(&semaphoreInfo, nullptr, &semaphore) == vk::Result::eSuccess ? "Successfully created semaphore" : "Failed to create semaphore");
    return semaphore;
}

vk::Fence Vulkan::createFence(vk::Device& device)
{
    vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);
    vk::Fence fence;
    Log::debug("{}\n", device.createFence(&fenceInfo, nullptr, &fence) == vk::Result::eSuccess ? "Successfully created fence" : "Failed to create fence");
    return fence;
}
