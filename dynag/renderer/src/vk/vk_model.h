#ifndef VK_MODEL
#define VK_MODEL

#include "base/model.h"
#include "vk_pipeline.h"
#include "vk_device.h"
#include "vk_buffer.h"
#include "vk_descriptor.h"
#include "vk_material.h"

#include "base/includes.h"


namespace Vulkan
{
	class Model : public ModelBase
	{
	public:

		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const;
		};

		struct Mesh
		{
			Mesh(std::vector<Vertex> _vertices,
				std::vector<uint32_t> _indices,
				std::string _name);

			/*Mesh(const Mesh&) = default;
			Mesh& operator=(const Mesh&) = default;*/

			std::string name = "";

			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			std::shared_ptr<Material> m_material;
			bool m_useMaterial;

			std::shared_ptr<Buffer> m_vertexBuffer;
			uint32_t m_vertexCount;

			bool m_hasIndexBuffer = false;
			std::shared_ptr<Buffer> m_indexBuffer;
			uint32_t m_indexCount;
		};

		struct Builder
		{
			Builder(Device& _device, const std::string& _filepath);
			~Builder();

			Builder(const Builder&) = delete;
			Builder& operator=(const Builder&) = delete;

			void loadAssimpModel();
			std::vector<Mesh> m_meshes;
		private:
			Device& m_device;
			std::string filepath;
			std::string directory = "";
			void processNode(aiNode* node, const aiScene* scene);
			Mesh processMesh(aiMesh* mesh, const aiScene* scene);
			void loadMaterialTextures(Texture& texture, aiMaterial* mat, aiTextureType type, std::string typeName);
			void textureFromFile(Texture& texture, const char* path, const std::string& directory);
		};

		Model(Device& device, const std::string& filepath, 
			VkRenderPass renderPass,
			std::unique_ptr<DescriptorPool>* globalPool,
			std::unique_ptr<DescriptorSetLayout>* globalSetLayout);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		static std::unique_ptr<Model> createModelFromFile(Device& device, 
			const std::string& filepath, 
			VkRenderPass renderPass,
			std::unique_ptr<DescriptorPool>* globalPool,
			std::unique_ptr<DescriptorSetLayout>* globalSetLayout);
		static VkSampler createTextureSampler(Device& device);

		void render(
			VkCommandBuffer& commandBuffer,
			VkDescriptorSet& globalDescriptorSet,
			Buffer& useTextureBuffer,
			glm::mat4& modelMatrix,
			glm::mat3& normalMatrix);
		std::vector<Mesh>& getMeshes() { return m_builder->m_meshes; }
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		// Translate the model to `translation` points.
		void translate(glm::vec3 translation) {}

		// Rotate the model to that angle.
		void rotate(float angle, glm::vec3 rotation) {}

		// Scale the model
		void scale(glm::vec3 scaling) {}

	private:

		void bind(VkCommandBuffer commandBuffer, const Mesh& mesh);
		void draw(VkCommandBuffer commandBuffer, const Mesh& mesh);


		void createVertexBuffers(Mesh& mesh);
		void createIndexBuffers(Mesh& mesh);
		void createTextures(Mesh& mesh);
		void createTextureBuffer(Texture& texture, std::string type, VkImage& image, VkDeviceMemory& imageMemory);
		void createTextureImage(uint32_t width, uint32_t height, 
			VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		VkImageView createImageView(VkImage image, VkFormat format);

		bool m_modelHasTexture;

		Device& m_device;
		std::unique_ptr<Builder> m_builder;
		std::unique_ptr<DescriptorSetLayout>* m_globalSetLayout;
		std::unique_ptr<DescriptorSetLayout> m_materialSetLayout;
		std::unique_ptr<DescriptorPool>* m_globalPool;
		std::unique_ptr<Pipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;
	};
}
#endif // !VK_MODEL
