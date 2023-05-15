#pragma once
#include "stb_image.hpp"
#include <vulkan/vulkan.hpp>
namespace VulkanImage
{
	struct TextureInputChunk
	{
		vk::PhysicalDevice physicalDevice;
		vk::Device device;
		const char* filename;
		vk::CommandBuffer cmdBuffer;
		vk::Queue queue;
		vk::DescriptorSetLayout descriptorLayout;
		vk::DescriptorPool descriptorPool;
	};

	struct ImageInputChunk
	{
		vk::PhysicalDevice physicalDevice;
		vk::Device device;
		int width, height;
		vk::ImageTiling tiling;
		vk::ImageUsageFlags usage;
		vk::MemoryPropertyFlags memoryProperties;
	};

	struct ImageLayoutTransitionJob
	{
		vk::CommandBuffer cmdBuffer;
		vk::Queue queue;
		vk::Image image;
		vk::ImageLayout oldLayout, newLayout;
	};

	struct BufferImageCopyJob
	{
		vk::CommandBuffer cmdBuffer;
		vk::Queue queue;
		vk::Buffer srcBuffer;
		vk::Image dstImage;
		int width, height;
	};

	class Texture
	{
	public:
		Texture(TextureInputChunk& input);
		~Texture();
		void use(vk::CommandBuffer cmdBuffer, vk::PipelineLayout layout);
	private:
		int width;
		int height;
		int channels;
		vk::PhysicalDevice physicalDevice;
		vk::Device device;
		const char* filename;
		stbi_uc* pixels;

		vk::Image image;
		vk::DeviceMemory imageMemory;
		vk::ImageView imageView;
		vk::Sampler sampler;

		vk::DescriptorSetLayout descriptorlayout;
		vk::DescriptorSet descriptorSet;
		vk::DescriptorPool descriptorPool;

		vk::CommandBuffer cmdBuffer;
		vk::Queue queue;

		void load();
		void populate();
		void createView();
		void createSampler();
		void createDescriptorSet();
	};
	vk::Image createImage(ImageInputChunk input);
	vk::DeviceMemory createImageMemory(ImageInputChunk input, vk::Image image);
	void transitionImageLayout(ImageLayoutTransitionJob job);
	void copyBufferToImage(BufferImageCopyJob job);
	vk::ImageView createImageView(vk::Device device, vk::Image image, vk::Format format);
};