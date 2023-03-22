#include "base/images.h"
#include "stb/stb_image.h"

#include "vk_model.h"
#include "vk_utils.h"
#include "vk_descriptor.h"
#include "vk_swap_chain.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <cstring>
#include <unordered_map>

#include <iostream>

namespace std
{
	template <>
	struct hash < Vulkan::Model::Vertex >
	{
		size_t operator()(Vulkan::Model::Vertex const& vertex) const
		{
			size_t seed = 0;
			Vulkan::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace Vulkan
{

	struct SimplePushConstantData
	{
		glm::mat4 modelMatrix{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
	};

	std::vector<VkVertexInputBindingDescription>  Model::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription>  Model::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

		return attributeDescriptions;
	}

	bool Model::Vertex::operator==(const Vertex& other) const
	{
		return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
	}

	Model::Model(Device& device,
		const std::string& filepath,
		VkRenderPass renderPass,
		std::unique_ptr<DescriptorPool>* globalPool,
		std::unique_ptr<DescriptorSetLayout>* globalSetLayout) : m_device{ device }, m_globalPool{ globalPool }, m_globalSetLayout{ globalSetLayout }, m_modelHasTexture {false}
	{

		m_materialSetLayout = DescriptorSetLayout::Builder(m_device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		m_builder = std::make_unique<Builder>(m_device, filepath);
		m_builder->loadAssimpModel();

		for (size_t i = 0; i < m_builder->m_meshes.size(); i++)
		{
			createVertexBuffers(m_builder->m_meshes.at(i));
			createIndexBuffers(m_builder->m_meshes.at(i));
			createTextures(m_builder->m_meshes.at(i));
		}

		createPipelineLayout(m_globalSetLayout->get()->getDescriptorSetLayout());
		createPipeline(renderPass);

	}

	Model::~Model()
	{
		vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
	}

	std::unique_ptr<Model> Model::createModelFromFile(Device& device,
		const std::string& filepath,
		VkRenderPass renderPass,
		std::unique_ptr<DescriptorPool>* globalPool,
		std::unique_ptr<DescriptorSetLayout>* globalSetLayout)
	{
		return std::make_unique<Model>(device, filepath, renderPass, globalPool, globalSetLayout);
	}

	void Model::bind(VkCommandBuffer commandBuffer, const Mesh& mesh)
	{
		VkBuffer buffers[] = { mesh.m_vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (mesh.m_hasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, mesh.m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void Model::draw(VkCommandBuffer commandBuffer, const Mesh& mesh)
	{
		if (mesh.m_hasIndexBuffer)
		{
			vkCmdDrawIndexed(commandBuffer, mesh.m_indexCount, 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(commandBuffer, mesh.m_vertexCount, 1, 0, 0);
		}
	}

	void Model::render(
		VkCommandBuffer& commandBuffer,
		VkDescriptorSet& globalDescriptorSet,
		Buffer& useTextureBuffer,
		glm::mat4& modelMatrix,
		glm::mat3& normalMatrix
		)
	{
		m_pipeline->bind(commandBuffer);

		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0,
			1,
			&globalDescriptorSet,
			0,
			nullptr
		);

		SimplePushConstantData push{};
		push.modelMatrix = modelMatrix;
		push.normalMatrix = normalMatrix;

		vkCmdPushConstants(commandBuffer,
			m_pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(SimplePushConstantData),
			&push);

		for (size_t i = 0; i < m_builder->m_meshes.size(); i++)
		{
			bool useTexture = m_builder->m_meshes.at(i).m_material->textureDiffuse->data ? true : false;
			useTextureBuffer.writeToBuffer(&useTexture);
			useTextureBuffer.flush();
			if (m_builder->m_meshes.at(i).m_material->descriptorSet)
			{				
				vkCmdBindDescriptorSets(
					commandBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					m_pipelineLayout,
					1,
					1,
					&m_builder->m_meshes.at(i).m_material->descriptorSet,
					0,
					nullptr
				);
			}

			

			bind(commandBuffer, m_builder->m_meshes.at(i));
			draw(commandBuffer, m_builder->m_meshes.at(i));
		}
	}

	void Model::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> desriptorSetLayouts{ globalSetLayout };
		if (m_modelHasTexture)
			desriptorSetLayouts.push_back(m_materialSetLayout.get()->getDescriptorSetLayout());

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(desriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = desriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_device.device(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout!");
		}

	}

	void Model::createPipeline(VkRenderPass renderPass)
	{
		assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_pipelineLayout;
		m_pipeline = std::make_unique<Pipeline>(m_device,
			pipelineConfig,
			"C:/Users/gkcyk/Desktop/project/vulkan-learning/gve/resources/shaders/simple_shader.vert.spv",
			"C:/Users/gkcyk/Desktop/project/vulkan-learning/gve/resources/shaders/simple_shader.frag.spv");
	}

	void Model::createVertexBuffers(Mesh& mesh)
	{
		mesh.m_vertexCount = static_cast<uint32_t>(mesh.vertices.size());
		assert(mesh.m_vertexCount >= 3 && "Vertex count must me at least 3!");
		uint32_t vertexSize = sizeof(mesh.vertices[0]);
		VkDeviceSize bufferSize = vertexSize * mesh.m_vertexCount;

		Buffer stagingBuffer(m_device,
			vertexSize,
			mesh.m_vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)mesh.vertices.data());

		mesh.m_vertexBuffer = std::make_unique<Buffer>(m_device,
			vertexSize,
			mesh.m_vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


		m_device.copyBuffer(stagingBuffer.getBuffer(), mesh.m_vertexBuffer->getBuffer(), bufferSize);
	}

	void Model::createIndexBuffers(Mesh& mesh)
	{
		mesh.m_indexCount = static_cast<uint32_t>(mesh.indices.size());
		mesh.m_hasIndexBuffer = mesh.m_indexCount > 0;

		if (!mesh.m_hasIndexBuffer) return;

		uint32_t indicesSize = sizeof(mesh.indices[0]);
		VkDeviceSize bufferSize = indicesSize * mesh.m_indexCount;

		Buffer stagingBuffer(m_device,
			indicesSize,
			mesh.m_indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)mesh.indices.data());

		mesh.m_indexBuffer = std::make_unique<Buffer>(m_device,
			indicesSize,
			mesh.m_indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


		m_device.copyBuffer(stagingBuffer.getBuffer(), mesh.m_indexBuffer->getBuffer(), bufferSize);
	}

	void Model::createTextures(Mesh& mesh)
	{

		if (mesh.m_material->textureDiffuse->data != nullptr)
		{
			m_modelHasTexture = true;
			auto mat = mesh.m_material.get();
			createTextureBuffer(*mat->textureDiffuse.get(), "diffuse", mat->textureDiffuse->image, mat->textureDiffuse->imageMemory);
			mat->textureDiffuse->imageView = createImageView(mat->textureDiffuse->image, VK_FORMAT_R8G8B8A8_SRGB);
			mat->textureDiffuse->sampler = Model::createTextureSampler(m_device);
		}
		if (mesh.m_material->textureSpecular->data != nullptr)
		{
			auto mat = mesh.m_material.get();
			createTextureBuffer(*mat->textureSpecular.get(), "specular", mat->textureSpecular->image, mat->textureSpecular->imageMemory);
			mat->textureSpecular->imageView = createImageView(mat->textureSpecular->image, VK_FORMAT_R8G8B8A8_SRGB);
			mat->textureSpecular->sampler = Model::createTextureSampler(m_device);
		}
		if (mesh.m_material->textureNormal->data != nullptr)
		{
			auto mat = mesh.m_material.get();
			createTextureBuffer(*mat->textureNormal.get(), "normal", mat->textureNormal->image, mat->textureNormal->imageMemory);
			mat->textureNormal->imageView = createImageView(mat->textureNormal->image, VK_FORMAT_R8G8B8A8_SRGB);
			mat->textureNormal->sampler = Model::createTextureSampler(m_device);
		}
		if (mesh.m_material->textureHeight->data != nullptr)
		{
			auto mat = mesh.m_material.get();
			createTextureBuffer(*mat->textureHeight.get(), "height", mat->textureHeight->image, mat->textureHeight->imageMemory);
			mat->textureHeight->imageView = createImageView(mat->textureHeight->image, VK_FORMAT_R8G8B8A8_SRGB);
			mat->textureHeight->sampler = Model::createTextureSampler(m_device);
		}

	}

	void Model::createTextureImage(uint32_t width, uint32_t height,
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		m_device.createImageWithInfo(imageInfo, properties, image, imageMemory);
	}

	VkSampler Model::createTextureSampler(Device& device)
	{
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(device.physicalDevice(), &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		VkSampler m_textureSampler;
		if (vkCreateSampler(device.device(), &samplerInfo, nullptr, &m_textureSampler) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture sampler!");
		}

		return m_textureSampler;
	}

	void Model::createTextureBuffer(Texture& texture, std::string type, VkImage& image, VkDeviceMemory& imageMemory)
	{
		Buffer stagingBuffer(m_device,
			texture.pixelSize,
			texture.imageSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)texture.data);

		if (texture.data)
		{
			stbi_image_free(texture.data);
		}
		else
		{
			stbi_image_free(texture.data);
			std::string error = "Texture failed to load ";
			error += type + " texture!";
			throw std::runtime_error(error);
		}


		createTextureImage(texture.width, texture.height,
			VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			image, imageMemory);

		m_device.transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		m_device.copyBufferToImage(stagingBuffer.getBuffer(), image, texture.width, texture.height, 1);
		m_device.transitionImageLayout(image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	}

	VkImageView Model::createImageView(VkImage image, VkFormat format)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(m_device.device(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture image view!");
		}

		return imageView;
	}

	Model::Builder::Builder(Device& _device, const std::string& _filepath)
		: m_device{ _device }, filepath{ _filepath }
	{
	}

	Model::Builder::~Builder()
	{
		for (auto& mesh : m_meshes)
		{
			if (mesh.m_material->textureDiffuse->data) vkDestroySampler(m_device.device(), mesh.m_material->textureDiffuse->sampler, nullptr);
			if (mesh.m_material->textureSpecular->data) vkDestroySampler(m_device.device(), mesh.m_material->textureSpecular->sampler, nullptr);
			if (mesh.m_material->textureNormal->data) vkDestroySampler(m_device.device(), mesh.m_material->textureNormal->sampler, nullptr);
			if (mesh.m_material->textureHeight->data) vkDestroySampler(m_device.device(), mesh.m_material->textureHeight->sampler, nullptr);
		}
	}

	void Model::Builder::loadAssimpModel()
	{
		// Read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);

		// Check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			std::string error = "Assimp : ";
			error += importer.GetErrorString();
			throw std::runtime_error(error);
		}

		// Retrieve the directory path of the filepath
		directory = filepath.substr(0, filepath.find_last_of('/'));

		// Process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
	}

	void Model::Builder::processNode(aiNode* node, const aiScene* scene)
	{
		// Process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// The node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			auto m = processMesh(mesh, scene);
			m_meshes.push_back(m);
		}

		// Process each of the children nodes.
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}

	Model::Mesh Model::Builder::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// Data to fill
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		Texture textures(m_device, nullptr, 0, 0,0,0,0);

		// walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // Declaring a placeholder vector since assimp uses 
			// its own vector class that doesn't directly convert to glm's vec3 
			// class so we transfer the data to this placeholder glm::vec3 first.

			// Positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.position = vector;

			// Normals
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.normal = vector;
			}

			if (mesh->HasVertexColors(i))
			{
				auto colors = mesh->mColors[i];
				vector.x = colors->r;
				vector.y = colors->g;
				vector.z = colors->b;
				vertex.color = vector;
			}
			else
				vertex.color = glm::vec3{ 1.0f };

			// Texture coordinates
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				// A vertex can contain up to 8 different texture coordinates. 
				// Thus make the assumption that it won't use models where a 
				// vertex can have multiple texture coordinates, so always take 
				// the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.uv = vec;
			}
			else
				vertex.uv = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}

		// Retrieve each of the mesh's faces and the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			// Retrieve all indices of the face and store them in the indices vector.
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// Process materials
		aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];

		// A convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN    

		Material gveMaterial;

		// 1. Diffuse maps
		Texture diffuseMap(m_device);
		loadMaterialTextures(diffuseMap, aiMaterial, aiTextureType_DIFFUSE, "texture_diffuse");
		gveMaterial.setDiffuse(diffuseMap);

		// 2. Specular maps
		Texture specularMap(m_device);
		loadMaterialTextures(specularMap, aiMaterial, aiTextureType_SPECULAR, "texture_specular");
		gveMaterial.setSpecular(specularMap);
		
		// 3. Normal maps
		Texture normalMap(m_device);
		loadMaterialTextures(normalMap, aiMaterial, aiTextureType_HEIGHT, "texture_normal");
		gveMaterial.setNormal(normalMap);
		
		// 4. Height maps
		Texture heightMap(m_device);
		loadMaterialTextures(heightMap, aiMaterial, aiTextureType_AMBIENT, "texture_height");
		gveMaterial.setHeight(heightMap);
		
		// 5. Opacity
		//float opacity_val;
		//aiMaterial->Get(AI_MATKEY_OPACITY, opacity_val);
		//opacity.push_back(opacity_val);

		Mesh _mesh(vertices, indices, mesh->mName.C_Str());
		_mesh.m_material = std::make_shared<Material>(gveMaterial);
		_mesh.m_useMaterial = diffuseMap.data ? true : false;
		return _mesh;
	}

	void Model::Builder::loadMaterialTextures(Texture& texture, aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			textureFromFile(texture, str.C_Str(), directory);
		}
	}

	void Model::Builder::textureFromFile(Texture& texture, const char* path, const std::string& directory)
	{
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		int width, height, channel;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channel, STBI_rgb_alpha);

		if (!data)
		{
			std::string error = "Texture failed to load : " + filename;
			throw std::runtime_error(error);
		}

		VkDeviceSize pixelSize = sizeof(data[0]);
		uint32_t imageSize = width * height * STBI_rgb_alpha;

		texture.data = data; 
		texture.imageSize = imageSize, 
		texture.pixelSize = pixelSize, 
		texture.width = static_cast<uint32_t>(width),
		texture.height = static_cast<uint32_t>(height),
		texture.channel = static_cast<uint32_t>(STBI_rgb_alpha);
	}
	
	Model::Mesh::Mesh(std::vector<Vertex> _vertices,
		std::vector<uint32_t> _indices,
		std::string _name)
		: vertices(_vertices),
		indices(_indices),
		name(_name)
	{
	}
}