#pragma once
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace Vulkan
{
	bool supported(std::vector<const char*>& instanceExtensions, std::vector<const char*>& vulkanLayers);
	bool isDeviceSuitable(const vk::PhysicalDevice& device, const std::vector<const char*>& deviceExtensions);
	VKAPI_ATTR vk::Bool32 VKAPI_CALL vkDebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity, vk::DebugUtilsMessageTypeFlagsEXT message_types, const vk::DebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);
	vk::Instance createInstance(const char* appName);
	vk::DebugUtilsMessengerEXT createDebug(vk::Instance& instance, vk::DispatchLoaderDynamic& dispatchLoaderDynamic);
	vk::PhysicalDevice createPhysicalDevice(vk::Instance& instance);
	vk::Device createLogicalDevice(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface);
}