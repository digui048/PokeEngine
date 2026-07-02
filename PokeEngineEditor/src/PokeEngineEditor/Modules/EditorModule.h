#ifndef EDITOR_MODULE_H
#define EDITOR_MODULE_H

#include "Poke/Core/Module.h"

#include <memory>

namespace Poke
{
    class VertexArray;
    class VertexBuffer;
    class IndexBuffer;
    class Shader;

    class EditorModule : public Module
    {
    public:
        EditorModule();
        ~EditorModule() override;

        void OnInit() override;
        void OnUpdate(float dt) override;
        void OnImGuiRender() override;
        void OnShutdown() override;

    private:
        std::unique_ptr<VertexArray> m_vao;
        std::shared_ptr<VertexBuffer> m_vbo;
        std::shared_ptr<IndexBuffer> m_ebo;

        std::shared_ptr<Shader> m_shader;
    };
}

#endif