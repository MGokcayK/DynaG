#ifndef VK_MATERIAL
#define VK_MATERIAL

#include "vk_texture.h"

namespace Vulkan
{
	class Material
	{
	public:
		Material();
		~Material();

		Material(const Material&) = default;
		Material& operator=(const Material&) = default;

		void setDiffuse(Texture& diffuse);
		void setSpecular(Texture& specular);
		void setNormal(Texture& normal);
		void setHeight(Texture& height);

		std::shared_ptr<Texture> textureDiffuse;
		std::shared_ptr<Texture> textureSpecular;
		std::shared_ptr<Texture> textureNormal;
		std::shared_ptr<Texture> textureHeight;
		
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
	};
}
#endif // !VK_MATERIAL