#ifndef EDITOR_INTERFACE_H
#define EDITOR_INTERFACE_H

#include <string>

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

    protected:
        std::string m_name;
        bool m_isOpen = true;
    };
};

#endif