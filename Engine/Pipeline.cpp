#include "Pipeline.hpp"
#include "Shader.hpp"
#include "Log.hpp"
#include "Mesh.hpp"

namespace Vulkan
{
	vk::PipelineLayout createPipelineLayout(vk::Device& device, std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts)
	{
		vk::PipelineLayoutCreateInfo layoutInfo({}, descriptorSetLayouts, {});
		vk::PipelineLayout layout;
		Log::debug("{}\n", device.createPipelineLayout(&layoutInfo, nullptr, &layout) == vk::Result::eSuccess ? "Pipeline layout successfully created" : "Failed creation pipeline layout");
		return layout;
	}

	vk::RenderPass createRenderPass(vk::Device& device, vk::Format& swapchainFormat)
	{
		vk::AttachmentDescription attachmentDescription({}, swapchainFormat, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
		vk::AttachmentReference attachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
		vk::SubpassDescription subpassDescription({}, vk::PipelineBindPoint::eGraphics, {}, attachmentReference, {}, {}, {});
		vk::RenderPassCreateInfo renderPassCreateInfo({}, attachmentDescription, subpassDescription, {});
		vk::RenderPass renderPass;
		Log::debug("{}\n", device.createRenderPass(&renderPassCreateInfo, nullptr, &renderPass) == vk::Result::eSuccess ? "RenderPass successfully created" : "Failed creation renderpass");
		return renderPass;
	}

	PipelineOutBundle createPipeline(PipelineInBundle& specification)
	{
		vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

		//Vertex input
		vk::VertexInputBindingDescription bindingDescription = VulkanMesh::getPosColorBindingDescription();
		const std::vector<vk::VertexInputAttributeDescription>& attributeDescription = VulkanMesh::getPosColorAttributeDescriptions();
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo({}, bindingDescription, attributeDescription);
		graphicsPipelineCreateInfo.pVertexInputState = &vertexInputInfo;

		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo({}, vk::PrimitiveTopology::eTriangleList);
		graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;

		vk::ShaderModule vertexShader = createModule(specification.vertexFilepath, specification.device);
		vk::PipelineShaderStageCreateInfo vertexShaderInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, vertexShader, "main");
		shaderStages.push_back(vertexShaderInfo);

		vk::Viewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = specification.swapchainExtent.width;
		viewport.height = specification.swapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vk::Rect2D scissor;
		scissor.offset.x = 0.0f;
		scissor.offset.y = 0.0f;
		scissor.extent = specification.swapchainExtent;
		vk::PipelineViewportStateCreateInfo viewportState(vk::PipelineViewportStateCreateFlags(), viewport, scissor);
		graphicsPipelineCreateInfo.pViewportState = &viewportState;

		vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
		rasterizationInfo.flags = vk::PipelineRasterizationStateCreateFlags();
		rasterizationInfo.depthClampEnable = VK_FALSE;
		rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
		rasterizationInfo.lineWidth = 1.0f;
		rasterizationInfo.cullMode = vk::CullModeFlagBits::eBack;
		rasterizationInfo.frontFace = vk::FrontFace::eClockwise;
		graphicsPipelineCreateInfo.pRasterizationState = &rasterizationInfo;

		vk::ShaderModule fragmentShader = createModule(specification.fragmentFilepath, specification.device);
		vk::PipelineShaderStageCreateInfo fragmentShaderInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment, fragmentShader, "main");
		shaderStages.push_back(fragmentShaderInfo);

		graphicsPipelineCreateInfo.stageCount = shaderStages.size();
		graphicsPipelineCreateInfo.pStages = shaderStages.data();

		vk::PipelineMultisampleStateCreateInfo multisamplingInfo;
		multisamplingInfo.flags = vk::PipelineMultisampleStateCreateFlags();
		multisamplingInfo.sampleShadingEnable = VK_FALSE;
		multisamplingInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
		graphicsPipelineCreateInfo.pMultisampleState = &multisamplingInfo;

		vk::PipelineColorBlendAttachmentState colorBlendAttachment;
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB;
		colorBlendAttachment.blendEnable = VK_FALSE;
		vk::PipelineColorBlendStateCreateInfo colorBlending;
		colorBlending.flags = vk::PipelineColorBlendStateCreateFlags();
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = vk::LogicOp::eCopy;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;
		graphicsPipelineCreateInfo.pColorBlendState = &colorBlending;

		vk::PipelineLayout layout = createPipelineLayout(specification.device, specification.descriptorSetLayouts);
		graphicsPipelineCreateInfo.layout = layout;
		vk::RenderPass renderPass = createRenderPass(specification.device, specification.swapchainFormat);
		graphicsPipelineCreateInfo.renderPass = renderPass;

		graphicsPipelineCreateInfo.basePipelineHandle = nullptr;

		vk::Pipeline graphicsPipeline;
		auto result = specification.device.createGraphicsPipeline(nullptr, graphicsPipelineCreateInfo);
		Log::debug("{}\n", result.result == vk::Result::eSuccess ? "Pipeline successfully created" : "Failed creation pipeline");
		graphicsPipeline = result.value;
		PipelineOutBundle output;
		output.layout = layout;
		output.renderPass = renderPass;
		output.pipeline = graphicsPipeline;

		specification.device.destroyShaderModule(vertexShader);
		specification.device.destroyShaderModule(fragmentShader);
		return output;
	}
}