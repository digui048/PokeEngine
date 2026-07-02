#ifndef EDITOR_INTERFACE_H
#define EDITOR_INTERFACE_H

#include <string>

namespace Poke
{
    class EditorInterface
    {
    public:
        EditorInterface(const std::string &name) : m_Name(name) {}
        virtual ~EditorInterface() = default;

        virtual void OnInit() {}
        virtual void OnUpdate(float dt) {}
        virtual void OnImGuiRender() = 0;
        virtual void OnShutdown() {}

        const std::string &GetName() const { return m_Name; }
        bool &IsOpen() { return m_IsOpen; }

    protected:
        std::string m_Name;
        bool m_IsOpen = true;
    };
};

#endif