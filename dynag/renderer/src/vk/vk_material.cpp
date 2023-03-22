#include "vk_material.h"

namespace Vulkan
{
	Material::Material() 
		: textureDiffuse(nullptr),
		textureSpecular(nullptr),
		textureNormal(nullptr),
		textureHeight(nullptr)
	{
	}

	Material::~Material()
	{
	}

	void Material::setDiffuse(Texture& diffuse)
	{
		textureDiffuse = std::make_shared<Texture>(diffuse);
	}

	void Material::setSpecular(Texture& specular)
	{
		textureSpecular = std::make_shared<Texture>(specular);
	}

	void Material::setNormal(Texture& normal)
	{
		textureNormal = std::make_shared<Texture>(normal);
	}

	void Material::setHeight(Texture& height)
	{
		textureHeight = std::make_shared<Texture>(height);
	}
}