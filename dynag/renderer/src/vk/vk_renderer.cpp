#include "vk_renderer.h"

#include <cassert>
#include <stdexcept>
#include <iostream>
#include <array>

namespace Vulkan
{
	Renderer::Renderer(Window& window, Device& device) : m_window{ window }, m_device{ device }
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	Renderer::~Renderer()
	{
		freeCommandBuffers();
	}

	VkRenderPass Renderer::getSwapChainRenderPass() const
	{
		return m_swapChain->getRenderPass();
	}

	float Renderer::getAspectRatio() const
	{
		return m_swapChain->extentAspectRatio();
	}

	bool Renderer::isFrameInProgress() const
	{
		return m_isFrameStarted;
	}

	VkCommandBuffer Renderer::getCurrentCommandBuffer() const
	{
		assert(m_isFrameStarted && "Cannot get current command buffer when frame is not in progress!");
		return m_commandBuffer[m_currentFrameIndex];
	}

	int Renderer::getFrameIndex() const
	{
		assert(m_isFrameStarted && "Cannot get frame index when frame is not in progress!");
		return m_currentFrameIndex;
	}

	VkCommandBuffer Renderer::beginFrame()
	{
		assert(!m_isFrameStarted && "Can't call beginFrame method when frame is already in progress!");

		auto result = m_swapChain->acquireNextImage(&m_currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		m_isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin command buffer!");
		}

		return commandBuffer;
	}

	void Renderer::endFrame()
	{
		assert(m_isFrameStarted && "Can't call endFrame method when frame is not in progress!");
		
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer!");
		}

		auto result = m_swapChain->submitCommandBuffers(&commandBuffer, &m_currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR ||
			result == VK_SUBOPTIMAL_KHR ||
			m_window.wasWindowResize())
		{
			m_window.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image!");
		}

		m_isFrameStarted = false;
		m_currentFrameIndex = (m_currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(m_isFrameStarted && "Can't call beginSwapChainRenderPass method when frame is not in progress!");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from different frame!");
		
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = m_swapChain->getRenderPass();
		renderPassBeginInfo.framebuffer = m_swapChain->getFrameBuffer(m_currentImageIndex);

		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = m_swapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.21f, 0.11f, 0.5f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		auto scExt = m_swapChain->getSwapChainExtent();
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(scExt.height);
		viewport.width = static_cast<float>(scExt.width);
		viewport.height = -static_cast<float>(scExt.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0},scExt };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}

	void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(m_isFrameStarted && "Can't call endSwapChainRenderPass method when frame is not in progress!");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from different frame!");
		vkCmdEndRenderPass(commandBuffer);
	}

	void Renderer::createCommandBuffers()
	{
		m_commandBuffer.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_device.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffer.size());

		if (vkAllocateCommandBuffers(m_device.device(), &allocInfo, m_commandBuffer.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffer!");
		}
	}

	void Renderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(m_device.device(), m_device.getCommandPool(), static_cast<uint32_t>(m_commandBuffer.size()), m_commandBuffer.data());
		m_commandBuffer.clear();
	}

	void Renderer::recreateSwapChain()
	{
		auto extent = m_window.getExtend();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_window.getExtend();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_device.device());

		if (m_swapChain == nullptr)
		{
			m_swapChain = std::make_unique<SwapChain>(m_device, extent);
		}
		else
		{
			std::shared_ptr<SwapChain> oldSwapChain = std::move(m_swapChain);
			m_swapChain = std::make_unique<SwapChain>(m_device, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*m_swapChain.get()))
			{
				throw std::runtime_error("Swap chain image or depth format has changed!");
			}
		}
	}
}