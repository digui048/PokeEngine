#ifndef EDITOR_INTERFACE_H
#define EDITOR_INTERFACE_H

#include <string>
#include <imgui.h>

namespace Poke
{
    class EditorInterface
    {
    public:
        EditorInterface(const std::string &name) : m_name(name) {}
        virtual ~EditorInterface() = default;

        virtual void OnInit() {}
        virtual void OnUpdate(float dt) {}
        virtual void OnImGuiRender() = 0;
        virtual void OnShutdown() {}

        const std::string &GetName() const { return m_name; }
        bool &IsOpen() { return m_isOpen; }
        bool IsHovered() const { return m_isHovered; }

        bool IsInside(float x, float y)
        {
            return x >= m_windowPos.x && x <= (m_windowPos.x + m_windowSize.x) &&
                   y >= m_windowPos.y && y <= (m_windowPos.y + m_windowSize.y);
        }

    protected: 
        std::string m_name;
        bool m_isOpen = true;
        bool m_isHovered = false;

        ImVec2 m_windowPos = {0.0f, 0.0f};
        ImVec2 m_windowSize = {0.0f, 0.0f};
    };
};

#endif