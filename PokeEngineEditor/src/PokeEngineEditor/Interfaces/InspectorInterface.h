#ifndef INSPECTOR_INTERFACE_H
#define INSPECTOR_INTERFACE_H

#include "EditorInterface.h"

namespace Poke
{
    class TransformComponent;
    class MeshComponent;

    class InspectorInterface : public EditorInterface
    {
    public:
        InspectorInterface() : EditorInterface("Inspector") {}

        void OnInit() override {}
        void OnUpdate(float dt) override {}
        void OnImGuiRender() override;
        void OnShutdown() override {}

    private:
        void DrawTransformComponent(TransformComponent *transform);
        void DrawMeshComponent(MeshComponent *mesh);
    };
}

#endif