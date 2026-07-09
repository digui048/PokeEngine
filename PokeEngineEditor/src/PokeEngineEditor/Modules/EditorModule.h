#ifndef EDITOR_MODULE_H
#define EDITOR_MODULE_H

#include "Poke/Core/Module.h"

#include <memory>
#include <vector>

namespace Poke
{
    class VertexArray;
    class VertexBuffer;
    class IndexBuffer;
    class Shader;
    class Mesh;
    class EditorInterface;

    class EditorModule : public Module
    {
    public:
        EditorModule();
        ~EditorModule() override;

        void OnInit() override;
        void OnUpdate(float dt) override;
        void OnImGuiRender() override;
        void OnShutdown() override;

        template<typename T, typename... Args>
        void AddInterface(Args&&... args)
        {
            auto interface = std::make_shared<T>(std::forward<Args>(args)...);
            interface->OnInit();
            m_Interfaces.push_back(interface);
        }

    private:
        std::vector<std::shared_ptr<EditorInterface>> m_Interfaces;
    };
}

#endif