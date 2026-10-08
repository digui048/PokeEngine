#ifndef INSPECTOR_INTERFACE_H
#define INSPECTOR_INTERFACE_H

#include "EditorInterface.h"
#include "PokeEngineEditor/Asset/EditorAssetManager.h"

namespace Poke
{
    class TransformComponent;
    class MeshRendererComponent;

    class InspectorInterface : public EditorInterface
    {
    public:
        InspectorInterface(EditorAssetManager *assetManager)
            : EditorInterface("Inspector"), m_assetManager(assetManager) {}

        void OnInit() override {}
        void OnUpdate(float dt) override {}
        void OnImGuiRender() override;
        void OnShutdown() override {}

    private:
        void DrawTransformComponent(TransformComponent *transform);
        void DrawMeshComponent(MeshRendererComponent *mesh);

    private:
        EditorAssetManager *m_assetManager;
    };
}

#endif