#pragma once
#include <vulkan/vulkan.hpp>
#include "ObjectData.hpp"

namespace Vulkan
{
	struct PipelineInBundle
	{
		vk::Device device;
		std::string vertexFilepath;
		std::string fragmentFilepath;
		vk::Extent2D swapchainExtent;
		vk::Format swapchainFormat;
		std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
	};
	struct PipelineOutBundle
	{
		vk::PipelineLayout layout;
		vk::RenderPass renderPass;
		vk::Pipeline pipeline;
	};

	vk::PipelineLayout createPipelineLayout(vk::Device& device, std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts);
	vk::RenderPass createRenderPass(vk::Device& device, vk::Format& swapchainFormat);
	PipelineOutBundle createPipeline(PipelineInBundle& specification);
}