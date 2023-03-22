#pragma once

#include "vk_device.h"
#include "vk_swap_chain.h"

#include <string>
#include <vector>

namespace Vulkan
{
	struct PipelineConfigInfo 
	{
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		std::vector<VkVertexInputBindingDescription> bindingDescription;
		std::vector<VkVertexInputAttributeDescription> attributeDescription;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class Pipeline
	{
	public:
		Pipeline() = default;
		Pipeline(Device& device,
				 const PipelineConfigInfo& configInfo,
				 const std::string& vertFilePath,
				 const std::string& fragFilePath);
		
		~Pipeline();

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		
		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

	private: 
		static std::vector<char> readFile(const std::string& filePath);

		void createGraphicsPipeline(const std::string& vertFilePath,
									const std::string& fragFilePath,
									const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code,
								VkShaderModule* shaderModule);

		Device& m_device;
		VkPipeline m_graphicsPipeline;
		VkShaderModule m_vertShaderModule;
		VkShaderModule m_fragShaderModule;
	};
}