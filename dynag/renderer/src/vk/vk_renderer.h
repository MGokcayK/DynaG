#ifndef VK_RENDERER
#define VK_RENDERER

#include "vk_window.h"
#include "vk_swap_chain.h"

namespace Vulkan
{
	class Renderer
	{
	public:
		Renderer(Window& window, Device& device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const;
		float getAspectRatio() const;
		bool isFrameInProgress() const;
		VkCommandBuffer getCurrentCommandBuffer() const;

		int getFrameIndex() const;

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		Window& m_window;
		Device& m_device;
		std::unique_ptr<SwapChain> m_swapChain;
		std::vector<VkCommandBuffer> m_commandBuffer;

		uint32_t m_currentImageIndex;
		int m_currentFrameIndex{ 0 };
		bool m_isFrameStarted{ false };
	};
}
#endif // !VK_RENDERER