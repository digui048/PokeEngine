#ifndef EDITOR_MODULE_H
#define EDITOR_MODULE_H

#include "Poke/Core/Module.h"

#include <memory>
#include <vector>
#include <string>
#include <filesystem>

namespace Poke
{
    class EditorInterface;
    class Mesh;
    class Scene;
    class GameObject;
    class MeshRendererComponent;
    class UniformBuffer;
    class VulkanPipeline;
    class Texture;
    class Material;

    class EditorModule : public Module
    {
    public:
        EditorModule(const std::string& projectName, const std::filesystem::path& projectDir);
        ~EditorModule() override;

        void OnInit() override;
        void OnUpdate(float dt) override;
        void OnRender(VkCommandBuffer cmd) override;
        void OnImGuiRender() override;
        void OnShutdown() override;

        void OnFileDropped(const char* path, float x, float y) override;

        template <typename T, typename... Args>
        void AddInterface(Args &&...args)
        {
            auto interface = std::make_shared<T>(std::forward<Args>(args)...);
            interface->OnInit();
            m_Interfaces.push_back(interface);
        }

        template<typename T>
        T* GetInterface()
        {
            for (auto &interface : m_Interfaces)
            {
                if (T *typedInterface = dynamic_cast<T *>(interface.get()))
                    return typedInterface;
            }
            return nullptr;
        }

        void RenderWorld(VkCommandBuffer cmd);

    private:
        void RenderEntity(VkCommandBuffer cmd, GameObject *entity);

    private:
        std::vector<std::shared_ptr<EditorInterface>> m_Interfaces;
        std::string m_projectName;
        std::filesystem::path m_projectDir;

        std::unique_ptr<Scene> m_scene;
        GameObject *m_shibaEntity;

        std::shared_ptr<Texture> m_textureS;
        std::shared_ptr<Material> m_materialS;

        std::shared_ptr<VulkanPipeline> m_defaultPipeline;
    };
}

#endif