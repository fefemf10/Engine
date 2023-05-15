#include "Utils.hpp"
#include "Log.hpp"


bool VulkanUtils::QueueFamilyIndices::isComplete()
{
	return graphicsFamily.has_value() && presentFamily.has_value();
}
VulkanUtils::QueueFamilyIndices VulkanUtils::findQueueFamilies(const vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface)
{
	QueueFamilyIndices indices;
	const std::vector<vk::QueueFamilyProperties>& queueFamilies = physicalDevice.getQueueFamilyProperties();
	Log::debug("System can support {} queue families\n", queueFamilies.size());
	int i{};
	for (const vk::QueueFamilyProperties& queue : queueFamilies)
	{
		if (queue.queueFlags & vk::QueueFlagBits::eGraphics)
			indices.graphicsFamily = i;
		if (physicalDevice.getSurfaceSupportKHR(i, surface))
			indices.presentFamily = i;
		if (indices.isComplete())
			break;
		++i;
	}
	return indices;
}