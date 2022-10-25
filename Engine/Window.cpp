#include "Window.hpp"

import Log;

Window::Window(const std::string& title)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GL::loadSettings();
	if (GL::settings.fullscreen)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		window = glfwCreateWindow(glfwGetVideoMode(monitor)->width, glfwGetVideoMode(monitor)->height, title.c_str(), GL::settings.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
	}
	else
	{
		window = glfwCreateWindow(GL::settings.width, GL::settings.height, title.c_str(), nullptr, nullptr);
	}
	vulkanLayers.push_back("VK_LAYER_KHRONOS_validation");
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	for (size_t i = 0; i < glfwExtensionCount; i++)
		instanceExtensions.emplace_back(glfwExtensions[i]);
	instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	
	initVulkan();

	//ImGui::CreateContext();
	//ImGui_ImplGlfw_InitForVulkan(window, true);
	//ImGui_ImplVulkan_Init("#version 430 core");
}

Window::~Window()
{
	glfwDestroyWindow(window);
}

void Window::loop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		draw();
	}
}

void Window::draw()
{
	uint32_t acquireNextImage;
	if (device.acquireNextImageKHR(swapchain, UINT64_MAX, semaphoreImageAvailable, nullptr, &acquireNextImage) != vk::Result::eSuccess)
		throw std::runtime_error("An error in the vkAcquireNextImageKHR function");

	vk::PipelineStageFlags pipelineStageFlags = vk::PipelineStageFlagBits::eTransfer;

	vk::SubmitInfo submitInfo(semaphoreImageAvailable, pipelineStageFlags, cmdBuffers, semaphoreRenderingFinished);
	if (queue.queue.submit(1, &submitInfo, nullptr) != vk::Result::eSuccess)
		throw std::runtime_error("An error in the vkQueueSubmit function");

	vk::PresentInfoKHR presentInfo(semaphoreRenderingFinished, swapchain, acquireNextImage);
	if (queue.queue.presentKHR(&presentInfo) != vk::Result::eSuccess)
		throw std::runtime_error("An error in the vkQueuePresentKHR function");
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL Window::vkDebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
	vk::DebugUtilsMessageTypeFlagsEXT message_types, const vk::DebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
{
	switch (message_severity)
	{
	case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
		Log::verbose("{}", callback_data->pMessage);
		break;
	case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
		Log::info("{}", callback_data->pMessage);
		break;
	case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
		Log::warning("{}", callback_data->pMessage);
		break;
	case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
		Log::error("{}", callback_data->pMessage);
		break;
	default:
		break;
	}
	return VK_FALSE;
}

std::vector<uint32_t> Window::loadShader(const std::string& filepath)
{
	std::ifstream file(filepath, std::ios::binary | std::ios::in);
	if (file.is_open())
	{
		std::streampos filesize = file.seekg(0, file.end).tellg();
		file.seekg(file.beg, 0);
		std::vector<uint32_t> data(filesize>>2);
		file.read((char*)data.data(), filesize);
		file.close();
		return std::move(data);
	}
	else
		return {};
}

bool Window::isDeviceSuitable(const vk::PhysicalDevice& device)
{
	QueueFamilyIndices indices = findQueueFamilies(device);
	return indices.graphicsFamily.has_value();
}

QueueFamilyIndices Window::findQueueFamilies(vk::PhysicalDevice physicalDevice)
{
	QueueFamilyIndices indices;
	uint32_t queueFamiliesCount{};
	physicalDevice.getQueueFamilyProperties(&queueFamiliesCount, nullptr);
	std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamiliesCount);
	physicalDevice.getQueueFamilyProperties(&queueFamiliesCount, queueFamilies.data());
	for (size_t i = 0; i < queueFamiliesCount; i++)
	{
		VkBool32 presentationSupported = VK_FALSE;
		physicalDevice.getSurfaceSupportKHR(i, surface, &presentationSupported);
		if (presentationSupported && queueFamilies[i].queueFlags && VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
			break;
		}
	}
	return indices;
}

void Window::findSurfaceFormat()
{
	uint32_t surfaceFormatCount{};
	physicalDevice.getSurfaceFormatsKHR(surface, &surfaceFormatCount, nullptr);
	std::vector<vk::SurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
	physicalDevice.getSurfaceFormatsKHR(surface, &surfaceFormatCount, surfaceFormats.data());
	if (surfaceFormatCount == 1 && surfaceFormats[0].format == vk::Format::eUndefined)
	{
		surfaceFormat = {vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
		return;
	}
	else
	{
		for (const vk::SurfaceFormatKHR& format : surfaceFormats)
		{
			if (format.format == vk::Format::eR8G8B8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			{
				surfaceFormat = format;
				return;
			}
		}
	}
	if (surfaceFormat.format == vk::Format::eUndefined)
		surfaceFormat = surfaceFormats[0];
}

void Window::findPresentMode()
{
	uint32_t presentModeCount{};
	physicalDevice.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr);
	std::vector<vk::PresentModeKHR> presentModes(presentModeCount);
	physicalDevice.getSurfacePresentModesKHR(surface, &presentModeCount, presentModes.data());
	for (const vk::PresentModeKHR& present : presentModes)
		if (present == vk::PresentModeKHR::eMailbox)
			presentMode = present;
	if (presentMode == vk::PresentModeKHR::eImmediate)
		presentMode = vk::PresentModeKHR::eFifo;
}

void Window::findSwapchainImages()
{
	uint32_t swapchainImagesCount{};
	device.getSwapchainImagesKHR(swapchain, &swapchainImagesCount, nullptr);
	swapchainImages.resize(swapchainImagesCount);
	device.getSwapchainImagesKHR(swapchain, &swapchainImagesCount, swapchainImages.data());
}

bool Window::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vk::enumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<vk::LayerProperties> avaliableLayers(layerCount);
	vk::enumerateInstanceLayerProperties(&layerCount, avaliableLayers.data());
	for (const auto& layerName : vulkanLayers)
	{
		bool layerFound{};
		for (const vk::LayerProperties& layerProperties : avaliableLayers)
		{
			if (std::string_view(layerName)== std::string_view(layerProperties.layerName))
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
			return false;
	}
	return true;
}

void Window::initVulkan()
{
	createInstance();
	createDebug();
	createSwapchain();
	createRenderPass();
	createBuffers();
	createShaders();
	createPipeline();
}

void Window::createDebug()
{
	vk::DebugUtilsMessageSeverityFlagsEXT logseverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
	vk::DebugUtilsMessageTypeFlagsEXT logtype = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

	debugCreateInfo = vk::DebugUtilsMessengerCreateInfoEXT({}, logseverity, logtype, (PFN_vkDebugUtilsMessengerCallbackEXT)Window::vkDebugCallback);
	
	instance.createDebugUtilsMessengerEXT(&debugCreateInfo, nullptr, &debugUtilsMessengerEXT, vk::DispatchLoaderDynamic{instance, (PFN_vkGetInstanceProcAddr)instance.getProcAddr("vkCreateDebugUtilsMessengerEXT") });
}

void Window::createInstance()
{
	vk::ApplicationInfo applicationInfo(applicationName.c_str(), 1, engineName.c_str(), 1, VK_API_VERSION_1_2);
	vk::InstanceCreateInfo instanceInfo({}, &applicationInfo, vulkanLayers, instanceExtensions, (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo);
	instance = vk::createInstance(instanceInfo, nullptr);
	glfwCreateWindowSurface(instance, window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface));
	uint32_t deviceCount{};
	instance.enumeratePhysicalDevices(&deviceCount, nullptr);
	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support");
	}
	std::vector<vk::PhysicalDevice> physicalDevices(deviceCount);
	instance.enumeratePhysicalDevices(&deviceCount, physicalDevices.data());
	for (const auto& pd : physicalDevices)
	{
		if (isDeviceSuitable(pd))
		{
			this->physicalDevice = pd;
			queue = findQueueFamilies(pd);
		}
	}

	float queuePriority[]{1.f};
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo({}, queue.graphicsFamily.value(), queuePriority);
	vk::DeviceCreateInfo deviceCreateInfo({}, deviceQueueCreateInfo, vulkanLayers, deviceExtensions);
	if (physicalDevice.createDevice(&deviceCreateInfo, nullptr, &device) != vk::Result::eSuccess)
		throw std::runtime_error("An error in the vkCreateDevice function");
	device.getQueue(queue.graphicsFamily.value(), 0, &queue.queue);
}

void Window::createSwapchain()
{
	findSurfaceFormat();
	findPresentMode();

	physicalDevice.getSurfaceCapabilitiesKHR(surface, &surfaceCapabilities);

	vk::SwapchainCreateInfoKHR swapchainCreateInfo(
		{}, surface, surfaceCapabilities.maxImageCount < 3 ? surfaceCapabilities.maxImageCount : 3, surfaceFormat.format, surfaceFormat.colorSpace,
		surfaceCapabilities.currentExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, queue.graphicsFamily.value(),
		surfaceCapabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, presentMode, VK_TRUE, nullptr);
	if (device.createSwapchainKHR(&swapchainCreateInfo, nullptr, &swapchain) != vk::Result::eSuccess)
		throw std::runtime_error("An error in the vkCreateSwapchainKHR function");
	
	findSwapchainImages();

	vk::CommandPoolCreateInfo cmdPoolCreateInfo({}, queue.graphicsFamily.value());
	if (device.createCommandPool(&cmdPoolCreateInfo, nullptr, &queue.cmdPool) != vk::Result::eSuccess)
		throw std::runtime_error("An error in the vkCreateCommandPool function");

	cmdBuffers.resize(swapchainImages.size());

	vk::CommandBufferAllocateInfo cmdBufferAllocateInfo(queue.cmdPool, vk::CommandBufferLevel::ePrimary, cmdBuffers.size());
	if (device.allocateCommandBuffers(&cmdBufferAllocateInfo, cmdBuffers.data()) != vk::Result::eSuccess)
		throw std::runtime_error("An error in the vkAllocateCommandBuffers function");

	vk::CommandBufferBeginInfo cmdBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
	clearColorValue = std::array<float,4>{ 1.f, 0.5f, 0.f, 0.f };
	vk::ImageSubresourceRange imageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

	for (size_t i = 0; i < swapchainImages.size(); i++)
	{
		vk::ImageMemoryBarrier imageMemoryBarrierPresentToClear(vk::AccessFlagBits::eMemoryRead, vk::AccessFlagBits::eTransferWrite, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, queue.graphicsFamily.value(), queue.graphicsFamily.value(), swapchainImages[i], imageSubresourceRange);
		vk::ImageMemoryBarrier imageMemoryBarrierClearToPresent(vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eMemoryRead, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR, queue.graphicsFamily.value(), queue.graphicsFamily.value(), swapchainImages[i], imageSubresourceRange);
		cmdBuffers[i].begin(&cmdBufferBeginInfo);
		cmdBuffers[i].pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrierPresentToClear);
		cmdBuffers[i].clearColorImage(swapchainImages[i], vk::ImageLayout::eTransferDstOptimal, &clearColorValue, 1,&imageSubresourceRange);
		cmdBuffers[i].pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, 0, nullptr, 0, nullptr, 1, & imageMemoryBarrierClearToPresent);
		cmdBuffers[i].end();
	}

	vk::SemaphoreCreateInfo semaphoreCreateInfo;
	if ((device.createSemaphore(&semaphoreCreateInfo, nullptr, &semaphoreImageAvailable) != vk::Result::eSuccess) ||
	(device.createSemaphore(&semaphoreCreateInfo, nullptr, &semaphoreRenderingFinished) != vk::Result::eSuccess))
		throw std::runtime_error("An error in the vk::CreateSemaphore function");
}

void Window::createRenderPass()
{
	vk::AttachmentDescription attachmentDescription({}, surfaceFormat.format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);
	vk::AttachmentReference attachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
	vk::SubpassDescription subpassDescription({}, vk::PipelineBindPoint::eGraphics, {}, attachmentReference, {}, {}, {});
	vk::RenderPassCreateInfo renderPassCreateInfo({}, attachmentDescription, subpassDescription, {});
	renderPass = device.createRenderPass(renderPassCreateInfo);
	imageViews.resize(swapchainImages.size());
	framebuffers.resize(swapchainImages.size());
	for (size_t i = 0; i < swapchainImages.size(); i++)
	{
		auto& imageView = imageViews[i];
		auto& framebuffer = framebuffers[i];
		vk::ImageSubresourceRange imageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
		vk::ImageViewCreateInfo imageViewCreateInfo({}, swapchainImages[i], vk::ImageViewType::e2D, surfaceFormat.format, { vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA }, imageSubresourceRange);
		imageView = device.createImageView(imageViewCreateInfo);
		vk::FramebufferCreateInfo framebufferCreateInfo({}, renderPass, imageView, surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height, 1);
		framebuffer = device.createFramebuffer(framebufferCreateInfo);
	}
}

void Window::createBuffers()
{
	std::vector<glm::vec2> position{ {-0.5f, 0.5f}, {0.5f, 0.5f}, {0.f, -0.5f} };
	vk::BufferCreateInfo bufferCreateInfo({}, position.size() * sizeof(glm::vec2), vk::BufferUsageFlagBits::eVertexBuffer, vk::SharingMode::eExclusive, queue.graphicsFamily.value());
	buffer = device.createBuffer(bufferCreateInfo);
	vk::DeviceMemory deviceMemory;
	vk::MemoryRequirements memoryRequirements;
	device.getBufferMemoryRequirements(buffer, &memoryRequirements);
	
	physicalDeviceMemoryProperties = physicalDevice.getMemoryProperties();
	vk::MemoryAllocateInfo memoryAllocateInfo(memoryRequirements.size, [&]() {
		for (size_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
		{
			uint32_t bit = (1u << i);
			if ((memoryRequirements.memoryTypeBits & bit) != 0)
			{
				if ((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible)
				{
					return i;
				}
			}
		}
		throw std::runtime_error("failed to get corrent memory type");
		}());
	device.allocateMemory(&memoryAllocateInfo, nullptr, &deviceMemory);
	void* data;
	if (device.mapMemory(deviceMemory, 0, position.size() * sizeof(glm::vec2), {}, &data) != vk::Result::eSuccess)
		throw std::runtime_error("failed to allocate device memory");
	memcpy_s(data, position.size() * sizeof(glm::vec3), position.data(), position.size() * sizeof(glm::vec3));
	device.unmapMemory(deviceMemory);
	device.bindBufferMemory(buffer, deviceMemory, 0);
}

void Window::createShaders()
{
	std::vector<uint32_t> vertData(loadShader("Shaders\\default.vert.spv"));
	vk::ShaderModuleCreateInfo shaderModuleCreateInfoVertex({}, vertData);
	device.createShaderModule(&shaderModuleCreateInfoVertex, nullptr, &shaderModuleVertex);

	std::vector<uint32_t> fragData(loadShader("Shaders\\default.frag.spv"));
	vk::ShaderModuleCreateInfo shaderModuleCreateInfoFragment({}, fragData);
	device.createShaderModule(&shaderModuleCreateInfoFragment, nullptr, &shaderModuleFragment);
}

void Window::createPipeline()
{
	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo({}, {}, {});
	pipelineLayout = device.createPipelineLayout(pipelineLayoutCreateInfo);
	vk::PipelineCache pipelineCache;
	vk::PipelineCacheCreateInfo pipelineCacheCreateInfo({}, {});
	device.createPipelineCache(pipelineCacheCreateInfo);

	std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos
	{
		vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, shaderModuleVertex, "main", nullptr),
		vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, shaderModuleFragment, "main", nullptr)
	};
	vk::VertexInputBindingDescription vertexInputBindingDescription(0, sizeof(glm::vec2));
	vk::VertexInputAttributeDescription vertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, 0);
	vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo({}, vertexInputBindingDescription, vertexInputAttributeDescription);
	
	vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eTriangleList, VK_FALSE);

	vk::Viewport viewport{
		0.f, 0.f,
		(float)surfaceCapabilities.currentExtent.width, (float)surfaceCapabilities.currentExtent.height,
		0.f, 0.f
	};
	vk::Rect2D scissor{
		{0, 0},
		{surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height}
	};
	vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo({}, viewport, scissor);
	
	vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo({}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise, VK_FALSE, 0.f, 0.f, 0.f, 1.f);

	vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo({}, vk::SampleCountFlagBits::e1, VK_FALSE, 0, nullptr, VK_FALSE, VK_FALSE);

	vk::StencilOpState stencilFront(vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::CompareOp::eAlways, 0, 0, 0);
	vk::StencilOpState stencilBack(vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::CompareOp::eAlways, 0, 0, 0);
	vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo({}, VK_FALSE, VK_FALSE, vk::CompareOp::eAlways, VK_FALSE, VK_FALSE, stencilFront, stencilBack, 0.f, 0.f);

	vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(VK_FALSE, vk::BlendFactor::eOne, vk::BlendFactor::eOne, vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eOne, vk::BlendOp::eAdd, vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB);
	vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo({}, VK_FALSE, vk::LogicOp::eClear, pipelineColorBlendAttachmentState, {1.f, 1.f, 1.f, 1.f});

	vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo({}, pipelineShaderStageCreateInfos, &pipelineVertexInputStateCreateInfo, &pipelineInputAssemblyStateCreateInfo, {}, &pipelineViewportStateCreateInfo, &pipelineRasterizationStateCreateInfo, &pipelineMultisampleStateCreateInfo, &pipelineDepthStencilStateCreateInfo, &pipelineColorBlendStateCreateInfo, {}, pipelineLayout, renderPass, 0, {}, 0);
	auto p = device.createGraphicsPipelines(pipelineCache, graphicsPipelineCreateInfo);
	pipeline = p.value[0];
	if (p.result != vk::Result::eSuccess)
		throw std::runtime_error("An error in the createGraphicsPipelines function");

	/*std::vector<vk::ClearValue>clr{ clearColorValue };

	for (size_t i = 0; i < swapchainImages.size(); i++)
	{
		vk::RenderPassBeginInfo renderPassBeginInfo(renderPass, framebuffers[i], vk::Rect2D({ 0, 0 }, {surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height}), clr);
		cmdBuffers[i].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
		cmdBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
		std::vector<vk::Buffer> vkB{buffer};
		std::vector<vk::DeviceSize> vkO{ 0 };
		cmdBuffers[i].bindVertexBuffers(0, vkB, vkO);
		cmdBuffers[i].draw(3, 1, 0, 0);
		cmdBuffers[i].endRenderPass();
	}*/
}
