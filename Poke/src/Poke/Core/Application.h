#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

namespace Poke
{
    class Window;
    class ImGuiManager;

    class VertexArray;
    class VertexBuffer;
    class IndexBuffer;
    class Shader;

    class Application
    {
    public:
        Application();
        virtual ~Application();

        static Application &GetInstance();

        void Run();

        Window *GetWindow() const { return m_window.get(); }

    protected:
        virtual void OnInit() {}
        virtual void OnUpdate(float dt) {}
        virtual void OnShutdown() {}

    private:
        void PollEvents();

    private:
        static Application *s_Instance;
        std::unique_ptr<Window> m_window;
        std::unique_ptr<ImGuiManager> m_imguiManager;
        bool m_Running = true;

        std::unique_ptr<VertexArray> m_vao;
        std::shared_ptr<VertexBuffer> m_vbo;
        std::shared_ptr<IndexBuffer> m_ebo;

        std::shared_ptr<Shader> m_shader;
    };
}

#endif