#pragma once
#include <iostream>
#include <fstream>
#include <optional>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GL.hpp"
#include "../IMGUI/imgui.h"
#include "../IMGUI/imgui_impl_glfw.h"
#include "../IMGUI/imgui_impl_vulkan.h"

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	vk::Queue queue;
	vk::CommandPool cmdPool;
};

class Window
{
public:
	Window(const std::string& title);
	~Window();
	void loop();
	void draw();
private:
	static VKAPI_ATTR vk::Bool32 VKAPI_CALL vkDebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
		vk::DebugUtilsMessageTypeFlagsEXT message_types, const vk::DebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);
	std::vector<uint32_t> loadShader(const std::string& filepath);
	bool isDeviceSuitable(const vk::PhysicalDevice& device);
	QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice physicalDevice);
	void findSurfaceFormat();
	void findPresentMode();
	void findSwapchainImages();
	bool checkValidationLayerSupport();
	void initVulkan();
	void createDebug();
	void createInstance();
	void createSwapchain();
	void createRenderPass();
	void createBuffers();
	void createShaders();
	void createPipeline();
	std::string applicationName;
	std::string engineName;
	GLFWwindow* window;
	std::vector<const char*> vulkanLayers;
	bool enableVulkanLayers{};
	std::vector<const char*> instanceExtensions;
	std::vector<const char*> deviceExtensions;
	std::vector<vk::Image> swapchainImages;
	std::vector<vk::CommandBuffer> cmdBuffers;
	vk::ClearColorValue clearColorValue;
	vk::DebugUtilsMessengerEXT debugUtilsMessengerEXT;
	vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;

	vk::Instance instance;
	vk::PhysicalDevice physicalDevice;
	vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
	vk::Device device;
	QueueFamilyIndices queue;
	vk::SurfaceCapabilitiesKHR surfaceCapabilities;
	vk::SwapchainKHR swapchain;
	vk::SurfaceFormatKHR surfaceFormat;
	vk::SurfaceKHR surface;
	vk::PresentModeKHR presentMode;

	vk::Semaphore semaphoreImageAvailable;
	vk::Semaphore semaphoreRenderingFinished;

	vk::RenderPass renderPass;
	std::vector<vk::ImageView> imageViews;
	std::vector<vk::Framebuffer> framebuffers;

	vk::ShaderModule shaderModuleVertex;
	vk::ShaderModule shaderModuleFragment;

	vk::Buffer buffer;
	vk::PipelineLayout pipelineLayout;
	vk::Pipeline pipeline;
};