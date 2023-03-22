#include "vk_texture.h"

namespace Vulkan
{
	Texture::Texture(Device& _device) : device(_device)
	{
	}

	Texture::Texture(Device& _device, unsigned char* _data, uint32_t _size, VkDeviceSize _pixelSize, uint32_t _width, uint32_t _height, uint32_t _channel)
		: 
		device(_device),
		data(_data),
		imageSize(_size),
		pixelSize(_pixelSize),
		width(_width),
		height(_height),
		channel(_channel)
	{
	}

	Texture::~Texture()
	{
		vkDestroyImageView(device.device(), imageView, nullptr);
		vkDestroyImage(device.device(), image, nullptr);
		vkFreeMemory(device.device(), imageMemory, nullptr);
	}
}