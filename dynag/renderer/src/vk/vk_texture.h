#ifndef VK_TEXTURE
#define VK_TEXTURE

#include "vk_device.h"

#include <memory>

namespace Vulkan
{
	class Texture
	{
	public:
		Texture(Device& _device);
		Texture(Device& device,
			unsigned char* _data,
			uint32_t _size,
			VkDeviceSize _pixelSize,
			uint32_t _width,
			uint32_t _height,
			uint32_t _channel);
		~Texture();

		Texture(const Texture&) = default;
		Texture& operator=(const Texture&) = default;
		
		unsigned char* data = 0;
		VkDeviceSize pixelSize = 0;
		uint32_t imageSize = 0;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t channel = 0;

		Device& device;
		VkImage image = VK_NULL_HANDLE;
		VkDeviceMemory imageMemory = VK_NULL_HANDLE;
		VkImageView imageView = VK_NULL_HANDLE;
		VkSampler sampler = VK_NULL_HANDLE;

	};
}
#endif // !VK_TEXTURE
