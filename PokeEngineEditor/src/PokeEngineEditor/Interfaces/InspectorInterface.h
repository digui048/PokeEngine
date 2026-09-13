#ifndef INSPECTOR_INTERFACE_H
#define INSPECTOR_INTERFACE_H

#include "EditorInterface.h"

namespace Poke
{
    class InspectorInterface : public EditorInterface
    {
    public:
        InspectorInterface() : EditorInterface("Inspector") {}
        
        void OnInit() override {}
        void OnUpdate(float dt) override {}
        void OnImGuiRender() override;
        void OnShutdown() override {}
    };
}

#endif