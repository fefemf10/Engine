#include "SingleTimeJob.hpp"

void VulkanUtils::beginJob(vk::CommandBuffer cmdBuffer)
{
    cmdBuffer.reset();
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmdBuffer.begin(beginInfo);
}

void VulkanUtils::endJob(vk::CommandBuffer cmdBuffer, vk::Queue queue)
{
    cmdBuffer.end();
    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;
    queue.submit(submitInfo);
    queue.waitIdle();
}
