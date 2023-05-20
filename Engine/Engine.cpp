#include "Engine.hpp"
#include "Log.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

import GL;

Engine::Engine(int width, int height, GLFWwindow* window) : width(width), height(height), window(window)
{
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	initVulkan();
}
Engine::~Engine()
{
	device.waitIdle();

	device.destroyCommandPool(cmdPool);

	device.destroyPipeline(pipeline);
	device.destroyPipelineLayout(layout);
	device.destroyRenderPass(renderPass);
	destroySwapchain();

	device.destroyDescriptorSetLayout(frameDescriptorSetLayout);
	device.destroyDescriptorSetLayout(meshDescriptorSetLayout);
	device.destroyDescriptorPool(meshDescriptorPool);

	delete meshes;
	for (const auto& [key, texture] : materials)
	{
		delete texture;
	}

	device.destroy();
	if constexpr (debug)
		instance.destroyDebugUtilsMessengerEXT(debugUtilsMessengerEXT, nullptr, dispatchLoaderDynamic);
	instance.destroySurfaceKHR(surface);
	instance.destroy();
	glfwDestroyWindow(window);
	glfwTerminate();
}
void Engine::render(Scene* scene)
{
	device.waitForFences(1, &swapchainFrames[frameNumber].inFlight, VK_TRUE, UINT64_MAX);
	device.resetFences(1, &swapchainFrames[frameNumber].inFlight);

	uint32_t imageIndex;
	try
	{
		vk::ResultValue acquire = device.acquireNextImageKHR(swapchain, UINT64_MAX, swapchainFrames[frameNumber].imageAvailable, nullptr);
		imageIndex = acquire.value;
	}
	catch (vk::OutOfDateKHRError error) {
		Log::debug("Recreate\n");
		recreateSwapchain();
		return;
	}
	catch (vk::IncompatibleDisplayKHRError error) {
		Log::debug("Recreate\n");
		recreateSwapchain();
		return;
	}
	catch (vk::SystemError error) {
		Log::debug("Failed to acquire swapchain image\n");
	}

	vk::CommandBuffer cmdBuffer = swapchainFrames[frameNumber].cmdBuffer;
	cmdBuffer.reset();
	createFrame(imageIndex, scene);
	recordDrawCoommands(cmdBuffer, imageIndex, scene);

	vk::Semaphore waitSemaphores[] = { swapchainFrames[frameNumber].imageAvailable };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	vk::Semaphore signalSemaphores[] = { swapchainFrames[frameNumber].renderFinished };
	vk::SubmitInfo submitInfo(waitSemaphores, waitStages, cmdBuffer, signalSemaphores);
	graphicsQueue.submit(1, &submitInfo, swapchainFrames[frameNumber].inFlight);
	//Log::debug("{}\n", graphicsQueue.submit(1, &submitInfo, inFlightFence) == vk::Result::eSuccess ? "Successfully submit queue" : "Failed to submit queue");

	vk::PresentInfoKHR presentInfo(signalSemaphores, swapchain, imageIndex);
	vk::Result present;

	try {
		present = presentQueue.presentKHR(presentInfo);
	}
	catch (vk::OutOfDateKHRError error) {
		present = vk::Result::eErrorOutOfDateKHR;
	}

	if (present == vk::Result::eErrorOutOfDateKHR || present == vk::Result::eSuboptimalKHR) {
		Log::debug("Recreate\n");
		recreateSwapchain();
		return;
	}

	frameNumber = (frameNumber + 1) % maxFramesInFlight;
}
void Engine::initVulkan()
{
	//createInstance
	instance = Vulkan::createInstance(title.c_str());
	dispatchLoaderDynamic = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
	Log::debug("{}\n", glfwCreateWindowSurface(instance, window, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface)) == VK_SUCCESS ? "Successfully abstracted the GLFW surface for Vulkan" : "Failed to abstract the GLFW surface for Vulkan");
	debugUtilsMessengerEXT = Vulkan::createDebug(instance, dispatchLoaderDynamic);
	//createDevice
	physicalDevice = Vulkan::createPhysicalDevice(instance);
	device = Vulkan::createLogicalDevice(physicalDevice, surface);
	VulkanUtils::QueueFamilyIndices indices = VulkanUtils::findQueueFamilies(physicalDevice, surface);
	graphicsQueue = device.getQueue(indices.graphicsFamily.value(), 0);
	presentQueue = device.getQueue(indices.presentFamily.value(), 0);
	createSwapchain();
	frameNumber = 0;

	//frame descriptor
	Vulkan::DescriptorSetLayoutData bindings;
	bindings.count = 2;

	bindings.indices.push_back(0);
	bindings.types.push_back(vk::DescriptorType::eUniformBuffer);
	bindings.counts.push_back(1);
	bindings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

	bindings.indices.push_back(1);
	bindings.types.push_back(vk::DescriptorType::eStorageBuffer);
	bindings.counts.push_back(1);
	bindings.stages.push_back(vk::ShaderStageFlagBits::eVertex);
	frameDescriptorSetLayout = Vulkan::createDescriptorSetLayout(device, bindings);

	//mesh descriptor
	bindings.count = 1;

	bindings.indices[0] = 0;
	bindings.types[0] = vk::DescriptorType::eCombinedImageSampler;
	bindings.counts[0] = 1;
	bindings.stages[0] = vk::ShaderStageFlagBits::eFragment;

	meshDescriptorSetLayout = Vulkan::createDescriptorSetLayout(device, bindings);


	//createPipeline
	Vulkan::PipelineInBundle specification;
	specification.device = device;
	specification.vertexFilepath = "Shaders\\default.vert.spv";
	specification.fragmentFilepath = "Shaders\\default.frag.spv";
	specification.swapchainExtent = swapchainExtent;
	specification.swapchainFormat = swapchainFormat;
	specification.depthFormat = swapchainFrames[0].depthFormat;
	specification.descriptorSetLayouts = { frameDescriptorSetLayout, meshDescriptorSetLayout };
	Vulkan::PipelineOutBundle output = Vulkan::createPipeline(specification);
	layout = output.layout;
	renderPass = output.renderPass;
	pipeline = output.pipeline;

	createFramebuffer();

	cmdPool = Vulkan::createCmdPool(physicalDevice, device, surface);

	Vulkan::cmdBufferInputChunk cmdBufferInputChunk {device, cmdPool, swapchainFrames };
	mainCmdBuffer = Vulkan::createCmdBuffer(cmdBufferInputChunk);
	Vulkan::createFrameCmdBuffer(cmdBufferInputChunk);
	createSync();
	createAssets();
}

void Engine::recordDrawCoommands(vk::CommandBuffer& cmdBuffer, uint32_t imageIndex, Scene* scene)
{
	vk::CommandBufferBeginInfo beginInfo;
	cmdBuffer.begin(beginInfo);
	//Log::debug("{}\n", cmdBuffer.begin(&beginInfo) == vk::Result::eSuccess ? "Successfully begin recording command buffer" : "Failed to begin recording command buffer");
	vk::ClearValue clearColor({ 1.0f, 0.5f, 0.25f, 1.0f });
	vk::ClearValue clearDepth;
	clearDepth.depthStencil = vk::ClearDepthStencilValue({1.f, 0});
	std::vector<vk::ClearValue> clearColors({ clearColor, clearDepth });
	vk::RenderPassBeginInfo renderPassInfo(renderPass, swapchainFrames[imageIndex].framebuffer, vk::Rect2D({}, swapchainExtent), clearColors);
	cmdBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
	cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, swapchainFrames[imageIndex].descriptorSet, nullptr);
	createScene(cmdBuffer);
	uint32_t startInstance{};
	recordDrawCmdObjects(cmdBuffer, MeshType::TRIANGLE, startInstance, scene->trianglePos.size());
	recordDrawCmdObjects(cmdBuffer, MeshType::SQUARE, startInstance, scene->squarePos.size());
	recordDrawCmdObjects(cmdBuffer, MeshType::STAR, startInstance, scene->starPos.size());
	cmdBuffer.endRenderPass();
	cmdBuffer.end();
}

void Engine::recordDrawCmdObjects(vk::CommandBuffer& cmdBuffer, MeshType objectType, uint32_t& startInstance, uint32_t instanceCount)
{
	int indexCount = meshes->indexCounts.find(objectType)->second;
	int firstIndex = meshes->firstIndices.find(objectType)->second;
	materials[objectType]->use(cmdBuffer, layout);
	cmdBuffer.drawIndexed(indexCount, instanceCount, firstIndex, 0, startInstance);
	startInstance += instanceCount;
}

void Engine::createSwapchain()
{
	Vulkan::SwapchainBundle bundle = Vulkan::createSwapchainBundle(physicalDevice, device, surface, width, height);
	swapchain = bundle.swapchain;
	swapchainFormat = bundle.format;
	swapchainExtent = bundle.extent;
	swapchainFrames = bundle.frames;
	maxFramesInFlight = swapchainFrames.size();

	for (Vulkan::SwapchainFrame& frame : swapchainFrames)
	{
		frame.physicalDevice = physicalDevice;
		frame.device = device;
		frame.width = swapchainExtent.width;
		frame.height = swapchainExtent.height;
		frame.createDepthResources();
	}
}

void Engine::recreateSwapchain()
{
	width = 0;
	height = 0;
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}
	device.waitIdle();
	destroySwapchain();
	createSwapchain();
	createFramebuffer();
	createSync();
	Vulkan::cmdBufferInputChunk cmdBufferInputChunk {device, cmdPool, swapchainFrames };
	Vulkan::createFrameCmdBuffer(cmdBufferInputChunk);
}

void Engine::destroySwapchain()
{
	for (Vulkan::SwapchainFrame& frame : swapchainFrames)
	{
		frame.destroy();
	}
	device.destroySwapchainKHR(swapchain);
	device.destroyDescriptorPool(frameDescriptorPool);
}

void Engine::createFramebuffer()
{
	Vulkan::FramebufferInput framebufferInput;
	framebufferInput.device = device;
	framebufferInput.renderPass = renderPass;
	framebufferInput.swapchainExtent = swapchainExtent;
	Vulkan::createFramebuffers(framebufferInput, swapchainFrames);
}

void Engine::createSync()
{
	Vulkan::DescriptorSetLayoutData bindings;
	bindings.count = 2;
	bindings.types.push_back(vk::DescriptorType::eUniformBuffer);
	bindings.types.push_back(vk::DescriptorType::eStorageBuffer);

	frameDescriptorPool = Vulkan::createDescriptorPool(device, swapchainFrames.size(), bindings);

	for (Vulkan::SwapchainFrame& frame : swapchainFrames)
	{
		frame.inFlight = Vulkan::createFence(device);
		frame.imageAvailable = Vulkan::createSemaphore(device);
		frame.renderFinished = Vulkan::createSemaphore(device);

		frame.createDescriptorResources();
		frame.descriptorSet = Vulkan::allocateDescriptorSet(device, frameDescriptorPool, frameDescriptorSetLayout);
	}
}

void Engine::createAssets()
{
	meshes = new VertexManager();

	std::vector<float> vertices = {
		 0.0f, -0.1f, 0.0f, 1.0f, 0.0f, 0.5f, 0.0f,
		 0.1f, 0.1f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-0.1f, 0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};
	std::vector<uint32_t> indices = { 0, 1, 2 };
	MeshType type = MeshType::TRIANGLE;
	meshes->consume(type, vertices, indices);

	vertices = {
		-0.1f,  0.1f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.1f, -0.1f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 0.1f, -0.1f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.1f,  0.1f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f
	};
	indices = {0, 1, 2, 2, 3, 0};
	type = MeshType::SQUARE;
	meshes->consume(type, vertices, indices);

	vertices = {
		 -0.1f, -0.05f, 1.0f, 1.0f, 1.0f, 0.0f, 0.25f,
		-0.04f, -0.05f, 1.0f, 1.0f, 1.0f, 0.3f, 0.25f,
		-0.06f,   0.0f, 1.0f, 1.0f, 1.0f, 0.2f,  0.5f,
		  0.0f,  -0.1f, 1.0f, 1.0f, 1.0f, 0.5f,  0.0f,
		 0.04f, -0.05f, 1.0f, 1.0f, 1.0f, 0.7f, 0.25f,
		  0.1f, -0.05f, 1.0f, 1.0f, 1.0f, 1.0f, 0.25f,
		 0.06f,   0.0f, 1.0f, 1.0f, 1.0f, 0.8f,  0.5f,
		 0.08f,   0.1f, 1.0f, 1.0f, 1.0f, 0.9f,  1.0f,
		  0.0f,  0.02f, 1.0f, 1.0f, 1.0f, 0.5f,  0.6f,
		-0.08f,   0.1f, 1.0f, 1.0f, 1.0f, 0.1f,  1.0f 
	};
	indices = {
			0, 1, 2,
			1, 3, 4,
			2, 1, 4,
			4, 5, 6,
			2, 4, 6,
			6, 7, 8,
			2, 6, 8,
			2, 8, 9
	};
	type = MeshType::STAR;
	meshes->consume(type, vertices, indices);
	FinalizationChunk finalizationChunk;
	finalizationChunk.physicalDevice = physicalDevice;
	finalizationChunk.device = device;
	finalizationChunk.queue = graphicsQueue;
	finalizationChunk.cmdBuffer = mainCmdBuffer;
	meshes->finalize(finalizationChunk);

	const std::unordered_map<MeshType, const char*> filenames {
		{MeshType::TRIANGLE, "Models\\1\\1.jpg"},
		{MeshType::SQUARE, "Models\\1\\2.jpg"},
		{MeshType::STAR, "Models\\1\\3.jpg"}
	};

	Vulkan::DescriptorSetLayoutData bindings;
	bindings.count = 1;
	bindings.types.push_back(vk::DescriptorType::eCombinedImageSampler);

	meshDescriptorPool = Vulkan::createDescriptorPool(device, filenames.size(), bindings);

	VulkanImage::TextureInputChunk textureInfo;
	textureInfo.cmdBuffer = mainCmdBuffer;
	textureInfo.queue = graphicsQueue;
	textureInfo.physicalDevice = physicalDevice;
	textureInfo.device = device;
	textureInfo.descriptorLayout = meshDescriptorSetLayout;
	textureInfo.descriptorPool = meshDescriptorPool;

	for (const auto& [object, filename] : filenames)
	{
		textureInfo.filename = filename;
		materials[object] = new VulkanImage::Texture(textureInfo);
	}
}

void Engine::createScene(vk::CommandBuffer cmdBuffer)
{
	vk::Buffer vertexBuffers[] = { meshes->vertexBuffer.buffer };
	vk::DeviceSize offsets[] = { 0 };
	cmdBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
	cmdBuffer.bindIndexBuffer(meshes->indexBuffer.buffer, 0, vk::IndexType::eUint32);
}

void Engine::createFrame(uint32_t imageIndex, Scene* scene)
{
	Vulkan::SwapchainFrame& frame = swapchainFrames[imageIndex];
	glm::vec3 eye {1.f, 0.f, -1.f};
	glm::vec3 center {0.f, 0.f, 0.f};
	glm::vec3 up {0.f, 0.f, -1.f};
	glm::mat4 view = glm::lookAt(eye, center, up);

	glm::mat4 projection = glm::perspective(glm::radians(45.f), float(swapchainExtent.width) / swapchainExtent.height, 0.1f, 10.f);
	projection[1][1] *= -1;

	frame.cameraData.view = view;
	frame.cameraData.projection = projection;
	frame.cameraData.model = projection * view;
	memcpy(frame.cameraDataWriteLocation, &frame.cameraData, sizeof(Vulkan::MVP));

	size_t i{};
	for (const glm::vec3& pos : scene->trianglePos)
		frame.modelTransform[i++] = glm::translate(glm::mat4(1.f), pos);
	for (const glm::vec3& pos : scene->squarePos)
		frame.modelTransform[i++] = glm::translate(glm::mat4(1.f), pos);
	for (const glm::vec3& pos : scene->starPos)
		frame.modelTransform[i++] = glm::translate(glm::mat4(1.f), pos);
	memcpy(frame.modelBufferWriteLocation, frame.modelTransform.data(), i * sizeof(glm::mat4));
	frame.writeDescriptorSet();
}
