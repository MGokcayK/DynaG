#include "vk_app.h"

//#include "systems/simple_render_system.h"
//#include "systems/point_light_system.h"
//#include "user_input.h"
//#include "buffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <iostream>
#include <array>
#include <chrono>

namespace Vulkan
{
    Application::Application(const unsigned int _width,
        const unsigned int _height,
        const char* _title)
    {
        if (m_window == nullptr)
        {
            m_window = std::make_unique<Window>(_width, _height, _title);
            auto& window = *static_cast<Vulkan::Window*>(m_window.get());
            m_device = std::make_unique<Device>( window );
            m_renderer = std::make_unique<Renderer>( window, *m_device.get());
        }
        api = ApplicationAPI::kVulkan;
        std::cout << " API : " << getApiName() << std::endl;

        m_globalPool = DescriptorPool::Builder(*m_device.get())
            .setMaxSets(10)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        m_globalSetLayout = DescriptorSetLayout::Builder(*m_device.get())
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();
        m_materialSetLayout = DescriptorSetLayout::Builder(*m_device.get())
            .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();
                //loadGameObjects();
          //      m_window.setKeyCallback(keyCallback);
    }

    Application::~Application()
    {
    }

    Model* Application::createModel(const std::string& path, 
        std::string vertex_shader_file_path, 
        std::string fragment_shader_file_path)
    {
        return Model::createModelFromFile(*m_device.get(), path, m_renderer->getSwapChainRenderPass(), &m_globalPool, &m_globalSetLayout).get();
    }





    /*
void Application::run()
{
    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < uboBuffers.size(); i++)
    {
        uboBuffers[i] = std::make_unique<Buffer>(m_device,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->map();
    }

    std::vector<VkDescriptorSet> globalDescriptorSet(SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < globalDescriptorSet.size(); i++)
    {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();

        DescriptorWriter(*m_globalSetLayout, *m_globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSet.at(i));
    }

    Buffer useTextureBuffer(m_device, sizeof(bool), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    useTextureBuffer.map();
    auto useTextureBufferInfo = useTextureBuffer.descriptorInfo();
    for (auto& obj : m_gameObjects)
    {
        if (obj.second.m_model)
        {
            auto meshes = obj.second.m_model->getMeshes();
            for (size_t j = 0; j < meshes.size(); j++)
            {

                auto mat = meshes[j].m_material.get();
                if (mat->textureDiffuse->data)
                {
                    VkDescriptorImageInfo info{};
                    info.sampler = mat->textureDiffuse->sampler;
                    info.imageView = mat->textureDiffuse->imageView;
                    info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    DescriptorWriter(*m_materialSetLayout, *m_globalPool)
                        .writeImage(0, &info)
                        .writeBuffer(1, &useTextureBufferInfo)
                        .build(mat->descriptorSet);
                }
                //if (mat->textureSpecular->data)
                //{
                //    VkDescriptorImageInfo info{};
                //    info.sampler = mat->textureSpecular->sampler;
                //    info.imageView = mat->textureSpecular->imageView;
                //    info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                //    writer = writer->writeImage(1, &info);
                //}
                //if (mat->textureNormal->data)
                //{
                //    VkDescriptorImageInfo info{};
                //    info.sampler = mat->textureNormal->sampler;
                //    info.imageView = mat->textureNormal->imageView;
                //    info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                //    writer = writer->writeImage(2, &info);
                //}
                //if (mat->textureHeight->data)
                //{
                //    VkDescriptorImageInfo info{};
                //    info.sampler = mat->textureHeight->sampler;
                //    info.imageView = mat->textureHeight->imageView;
                //    info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                //    writer = writer->writeImage(3, &info);
                //}

            }
        }
    }


    SimpleRenderSystem simpleRenderSystem{ m_device, m_renderer.getSwapChainRenderPass(), m_globalSetLayout->getDescriptorSetLayout(), m_materialSetLayout->getDescriptorSetLayout()};
    Camera camera{};
    camera.setViewDirection(glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f });
    PointLightSystem pointLightSystem{ m_device, m_renderer.getSwapChainRenderPass(), m_globalSetLayout->getDescriptorSetLayout() };

    auto viewerObject = GameObject::createGameObject();
    viewerObject.m_transform.translation.y -= 1.0f;
    viewerObject.m_transform.translation.x -= 5.0f;
    viewerObject.m_transform.translation.z -= 5.0f;
    UserInput cameraController{};
    cameraController.m_yaw = glm::radians<float>(45.0f);

    auto currentTime = std::chrono::high_resolution_clock::now();

    auto windowPtr = m_window.getGLFWwindow();

    while (!m_window.shouldClose())
    {
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        frameTime = glm::min(frameTime, MAX_FRAME_TIME);

        cameraController.moveInPlaneXZ(windowPtr, frameTime, viewerObject);
        camera.setViewYXZ(viewerObject.m_transform.translation, viewerObject.m_transform.rotation);

        float aspect = m_renderer.getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 1000.0f);

        if (auto commandBuffer = m_renderer.beginFrame())
        {
            int frameIndex = m_renderer.getFrameIndex();
            FrameInfo frameInfo
            {
                frameIndex,
                frameTime,
                commandBuffer,
                camera,
                globalDescriptorSet[frameIndex],
                m_gameObjects
            };

            GlobalUbo ubo{};
            ubo.projection = camera.getProjection();
            ubo.view = camera.getView();
            ubo.inverseView = camera.getInverseView();
            pointLightSystem.update(frameInfo, ubo);
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();

            m_renderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(frameInfo, useTextureBuffer);
            pointLightSystem.render(frameInfo);
            m_renderer.endSwapChainRenderPass(commandBuffer);
            m_renderer.endFrame();
        }
    }

    vkDeviceWaitIdle(m_device.device());
}
    */

    /*
void Application::loadGameObjects()
{
    std::shared_ptr<Model> model = Model::createModelFromFile(m_device, "../resources/models/smooth_vase.obj", m_renderer.getSwapChainRenderPass(), & m_globalPool, &m_globalSetLayout);
    //model->createPipelineLayout(m_globalSetLayout->getDescriptorSetLayout(), m_materialSetLayout->getDescriptorSetLayout());
    //model->createPipeline(m_renderer.getSwapChainRenderPass());

    auto gameObject = GameObject::createGameObject();
    gameObject.m_model = model;
    gameObject.m_transform.translation = { 1.0f, 0.0f, 0.0f };
    gameObject.m_transform.scale = glm::vec3{ 10.0f };


    std::shared_ptr<Model> model_floor = Model::createModelFromFile(m_device, "../resources/models/quad.obj");

    auto gameObject2 = GameObject::createGameObject();
    gameObject2.m_model = model_floor;
    gameObject2.m_transform.translation = { 0.0f, 0.0f, 0.0f };
    gameObject2.m_transform.scale = glm::vec3{ 3.0f };


    std::shared_ptr<Model> helicopter = Model::createModelFromFile(m_device, "../resources/models/aw109/aw109.obj", m_renderer.getSwapChainRenderPass(), & m_globalPool, &m_globalSetLayout);
    //helicopter->createPipelineLayout(m_globalSetLayout->getDescriptorSetLayout(), m_materialSetLayout->getDescriptorSetLayout());
    //helicopter->createPipeline(m_renderer.getSwapChainRenderPass());

    auto helicopterObj = GameObject::createGameObject();
    helicopterObj.m_model = helicopter;
    helicopterObj.m_transform.translation = { 0.0f, -5.0f, 0.0f };

    auto pointLight1 = GameObject::makePointLight(100.2f);
    pointLight1.m_transform.translation = glm::vec3{ 0.0f, -1.0f, 0.0f };
    auto pointLight2 = GameObject::makePointLight(100.2f);
    pointLight2.m_transform.translation = glm::vec3{ -5.0f, -3.0f, -2.0f };

    m_gameObjects.emplace(gameObject.getID(), std::move(gameObject));
    //m_gameObjects.emplace(gameObject2.getID(), std::move(gameObject2));
    m_gameObjects.emplace(pointLight1.getID(), std::move(pointLight1));
    m_gameObjects.emplace(pointLight2.getID(), std::move(pointLight2));
    m_gameObjects.emplace(helicopterObj.getID(), std::move(helicopterObj));
    */

    /*
    void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        {
            if (Window::windowFocused)
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                Window::windowFocused = false;
            }
            else
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                Window::windowFocused = true;
            }
        }
    }

}
    */
}