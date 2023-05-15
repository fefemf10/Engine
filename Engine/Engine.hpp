#pragma once
#include "Vulkan.hpp"
#include "Utils.hpp"
#include "Swapchain.hpp"
#include "Pipeline.hpp"
#include "Framebuffer.hpp"
#include "Command.hpp"
#include "Sync.hpp"
#include "Descriptors.hpp"
#include "Scene.hpp"
#include "VertexManager.hpp"
#include "Image.hpp"

class Engine
{
public:
	Engine(int width, int height, GLFWwindow* window);
	~Engine();
	void render(Scene* scene);
private:
	void initVulkan();
	void recordDrawCoommands(vk::CommandBuffer& cmdBuffer, uint32_t imageIndex, Scene* scene);
	void recordDrawCmdObjects(vk::CommandBuffer& cmdBuffer, MeshType objectType, uint32_t& startInstance, uint32_t instanceCount);
	void createSwapchain();
	void recreateSwapchain();
	void destroySwapchain();
	void createFramebuffer();
	void createSync();
	void createAssets();
	void createScene(vk::CommandBuffer cmdBuffer);
	void createFrame(uint32_t imageIndex, Scene* scene);
	std::string applicationName;
	std::string engineName;
	GLFWwindow* window;
	int width;
	int height;
	std::string title = "Engine";
	Scene scene;
	std::vector<vk::CommandBuffer> cmdBuffers;
	vk::ClearColorValue clearColorValue;
	vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;

	vk::Instance instance;
	vk::DebugUtilsMessengerEXT debugUtilsMessengerEXT;
	vk::DispatchLoaderDynamic dispatchLoaderDynamic;
	vk::SurfaceKHR surface;

	vk::PhysicalDevice physicalDevice;
	vk::Device device;
	vk::Queue graphicsQueue;
	vk::Queue presentQueue;
	vk::SwapchainKHR swapchain;
	std::vector<Vulkan::SwapchainFrame> swapchainFrames;
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;
	vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

	vk::PipelineLayout layout;
	vk::RenderPass renderPass;
	vk::Pipeline pipeline;

	vk::CommandPool cmdPool;
	vk::CommandBuffer mainCmdBuffer;

	int maxFramesInFlight, frameNumber;

	vk::DescriptorSetLayout frameDescriptorSetLayout;
	vk::DescriptorPool frameDescriptorPool;
	vk::DescriptorSetLayout meshDescriptorSetLayout;
	vk::DescriptorPool meshDescriptorPool;

	vk::DescriptorSet descriptorSet;

	VertexManager* meshes;
	std::unordered_map<MeshType, VulkanImage::Texture*> materials;
};