#ifndef EDITOR_MODULE_H
#define EDITOR_MODULE_H

#include "Poke/Core/Module.h"

#include <memory>
#include <vector>

namespace Poke
{
    class EditorInterface;
    class Mesh;
    class Scene;
    class GameObject;
    class MeshComponent;
    class UniformBuffer;
    class VulkanPipeline;
    class VulkanTexture;

    class EditorModule : public Module
    {
    public:
        EditorModule();
        ~EditorModule() override;

        void OnInit() override;
        void OnUpdate(float dt) override;
        void OnRender(VkCommandBuffer cmd) override;
        void OnImGuiRender() override;
        void OnShutdown() override;

        template <typename T, typename... Args>
        void AddInterface(Args &&...args)
        {
            auto interface = std::make_shared<T>(std::forward<Args>(args)...);
            interface->OnInit();
            m_Interfaces.push_back(interface);
        }

        void RenderWorld(VkCommandBuffer cmd);

    private:
        void RenderEntity(VkCommandBuffer cmd, GameObject *entity);

    private:
        std::vector<std::shared_ptr<EditorInterface>> m_Interfaces;

        std::unique_ptr<Scene> m_scene;
        GameObject *m_shibaEntity;

        std::shared_ptr<VulkanPipeline> m_defaultPipeline;
        std::shared_ptr<VulkanTexture> m_texture;
    };
}

#endif