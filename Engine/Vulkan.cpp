#include "Vulkan.hpp"
#include <set>
#include "Utils.hpp"
#include "Log.hpp"

namespace Vulkan
{
	bool supported(std::vector<const char*>& instanceExtensions, std::vector<const char*>& vulkanLayers)
	{
		const std::vector<vk::ExtensionProperties>& supportedExtensions = vk::enumerateInstanceExtensionProperties();
		if constexpr (debug)
		{
			Log::debug("Device can support the following extensions:\n");
			for (const vk::ExtensionProperties& supportedExtension : supportedExtensions)
				Log::debug("\t{}\n", (const char*)supportedExtension.extensionName);
		}
		bool found;
		for (const char* extension : instanceExtensions)
		{
			found = false;
			for (const vk::ExtensionProperties& supportedExtension : supportedExtensions)
			{
				if (strcmp(extension, supportedExtension.extensionName) == 0)
				{
					found = true;
					Log::debug("Extension {} is supported!\n", extension);
				}
			}
			if (!found)
			{
				Log::debug("Extension {} is not supported!\n", extension);
				return false;
			}
		}

		const std::vector<vk::LayerProperties>& supportedLayers = vk::enumerateInstanceLayerProperties();
		if constexpr (debug)
		{
			Log::debug("Device can support the following layers:\n");
			for (const vk::LayerProperties& supportedLayer : supportedLayers)
				Log::debug("\t{}\n", (const char*)supportedLayer.layerName);
		}
		for (const char* layer : vulkanLayers)
		{
			found = false;
			for (const vk::LayerProperties& supportedLayer : supportedLayers)
			{
				if (strcmp(layer, supportedLayer.layerName) == 0)
				{
					found = true;
					Log::debug("Layer {} is supported!\n", layer);
				}
			}
			if (!found)
			{
				Log::debug("Layer {} is not supported!\n", layer);
				return false;
			}
		}
		return true;
	}
	bool isDeviceSuitable(const vk::PhysicalDevice& device, const std::vector<const char*>& deviceExtensions)
	{
		std::set<std::string_view> extensions(deviceExtensions.cbegin(), deviceExtensions.cend());
		Log::debug("Device can support extrensions:\n");
		for (const vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties())
		{
			Log::debug("\t{}\n", (const char*)extension.extensionName);
			extensions.erase(extension.extensionName);
		}
		const bool extensionsSupported{ extensions.empty() };
		Log::debug("{}\n", extensionsSupported ? "Device can support the requested extrensions!" : "Device can not support the requested extrensions!");
		return extensionsSupported;
	}
	VKAPI_ATTR vk::Bool32 VKAPI_CALL vkDebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity, vk::DebugUtilsMessageTypeFlagsEXT message_types, const vk::DebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
	{
		switch (message_severity)
		{
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
			Log::verbose("{}\n", callback_data->pMessage);
			break;
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
			Log::info("{}\n", callback_data->pMessage);
			break;
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
			Log::warning("{}\n", callback_data->pMessage);
			break;
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
			Log::error("{}\n", callback_data->pMessage);
			break;
		default:
			break;
		}
		return VK_FALSE;
	}
	vk::Instance createInstance(const char* appName)
	{
		uint32_t version{ 0 };
		vk::enumerateInstanceVersion(&version);

		Log::debug("System can support vulkan Variant: {}, Major: {}, Minor: {}, Patch: {}\n", VK_API_VERSION_VARIANT(version), VK_API_VERSION_MAJOR(version), VK_API_VERSION_MINOR(version), VK_API_VERSION_PATCH(version));

		version = VK_MAKE_API_VERSION(0, 1, 2, 0);

		const vk::ApplicationInfo applicationInfo(appName, 1, "", 1, version);

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> instanceExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		if constexpr (debug)
			instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		std::vector<const char*> vulkanLayers;
		if constexpr (debug)
			vulkanLayers.push_back("VK_LAYER_KHRONOS_validation");

		if (!supported(instanceExtensions, vulkanLayers))
			return nullptr;

		const vk::InstanceCreateInfo instanceInfo(vk::InstanceCreateFlags(), &applicationInfo, vulkanLayers, instanceExtensions);
		return vk::createInstance(instanceInfo);
	}
	vk::DebugUtilsMessengerEXT createDebug(vk::Instance& instance, vk::DispatchLoaderDynamic& dispatchLoaderDynamic)
	{
		if constexpr (debug)
		{
			const vk::DebugUtilsMessageSeverityFlagsEXT logseverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
			const vk::DebugUtilsMessageTypeFlagsEXT logtype = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
			const vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo = vk::DebugUtilsMessengerCreateInfoEXT(vk::DebugUtilsMessengerCreateFlagsEXT(), logseverity, logtype, (PFN_vkDebugUtilsMessengerCallbackEXT)vkDebugCallback, nullptr);
			return instance.createDebugUtilsMessengerEXT(debugCreateInfo, nullptr, dispatchLoaderDynamic);
		}
		return nullptr;
	}
	vk::PhysicalDevice createPhysicalDevice(vk::Instance& instance)
	{
		const std::vector<vk::PhysicalDevice>& physicalDevices = instance.enumeratePhysicalDevices();
		std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		for (const vk::PhysicalDevice& device : physicalDevices)
		{
			const vk::PhysicalDeviceProperties& properties = device.getProperties();
			Log::debug("Device name: {}\n", (const char*)properties.deviceName);
			Log::debug("Device type: {}\n", vk::to_string(properties.deviceType));
			if (isDeviceSuitable(device, deviceExtensions))
				return device;
		}
		return nullptr;
	}
	vk::Device createLogicalDevice(vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface)
	{
		VulkanUtils::QueueFamilyIndices indices = VulkanUtils::findQueueFamilies(physicalDevice, surface);
		std::vector<uint32_t> uniqueIndices = { indices.graphicsFamily.value() };
		if (indices.graphicsFamily.value() != indices.presentFamily.value())
			uniqueIndices.push_back(indices.presentFamily.value());

		std::vector<float> queuePriorities{ 1.f };
		std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
		for (uint32_t queueFamilyIndex : uniqueIndices)
			deviceQueueCreateInfos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), queueFamilyIndex, queuePriorities));
		vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();
		std::vector<const char*> vulkanLayers;
		if constexpr (debug)
			vulkanLayers.push_back("VK_LAYER_KHRONOS_validation");
		std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		vk::DeviceCreateInfo deviceCreateInfo(vk::DeviceCreateFlags(), deviceQueueCreateInfos, vulkanLayers, deviceExtensions, &deviceFeatures);
		vk::Device device;
		Log::debug("{}\n", physicalDevice.createDevice(&deviceCreateInfo, nullptr, &device) == vk::Result::eSuccess ? "GPU has been successfully abstracted" : "Device creation failed");
		return device;
	}
}