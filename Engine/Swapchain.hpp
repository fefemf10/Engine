#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "Memory.hpp"

namespace Vulkan
{
	struct MVP
	{
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;
	};
	struct SwapchainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};
	class SwapchainFrame
	{
	public:
		vk::PhysicalDevice physicalDevice;
		vk::Device device;

		vk::Image image;
		vk::ImageView imageView;
		vk::Framebuffer framebuffer;
		vk::Image depthBuffer;
		vk::DeviceMemory depthBufferMemory;
		vk::ImageView depthBufferView;
		vk::Format depthFormat;
		int width, height;

		vk::CommandBuffer cmdBuffer;


		vk::Semaphore imageAvailable, renderFinished;
		vk::Fence inFlight;

		MVP cameraData;
		VulkanUtils::Buffer cameraDataBuffer;
		void* cameraDataWriteLocation;

		std::vector<glm::mat4> modelTransform;
		VulkanUtils::Buffer modelBuffer;
		void* modelBufferWriteLocation;

		vk::DescriptorBufferInfo uniformBufferDescriptor;
		vk::DescriptorBufferInfo modelBufferDescriptor;
		vk::DescriptorSet descriptorSet;

		void createDescriptorResources();
		void createDepthResources();
		void writeDescriptorSet();
		void destroy();
	};
	struct SwapchainBundle
	{
		vk::SwapchainKHR swapchain;
		std::vector<SwapchainFrame> frames;
		vk::Format format;
		vk::Extent2D extent;
	};
	vk::SurfaceFormatKHR findSurfaceFormat(std::span<vk::SurfaceFormatKHR> formats);
	vk::PresentModeKHR findPresentMode(std::span<vk::PresentModeKHR> presentModes);
	vk::Extent2D findExtent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR& capabilities);
	SwapchainBundle createSwapchainBundle(vk::PhysicalDevice& physicalDevice, vk::Device& device, vk::SurfaceKHR& surface, uint32_t width, uint32_t height);
}