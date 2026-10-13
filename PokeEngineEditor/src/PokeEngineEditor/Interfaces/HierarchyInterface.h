#ifndef HIERARCHY_INTERFACE_H
#define HIERARCHY_INTERFACE_H

#include "EditorInterface.h"

#include <memory>

namespace Poke
{
    class Scene;
    class GameObject;

    class HierarchyInterface : public EditorInterface
    {
    public:
        HierarchyInterface(std::shared_ptr<Scene> scene) : EditorInterface("Hierarchy") { SetActiveScene(std::move(scene)); }

        void OnInit() override;
        void OnImGuiRender() override;

        static GameObject *GetSelectedEntity() { return s_selectedEntity; }
        static void SetSelectedEntity(GameObject *entity) { s_selectedEntity = entity; }

        static std::shared_ptr<Scene> GetActiveScene() { return s_activeScene; }
        static void SetActiveScene(std::shared_ptr<Scene> scene);

    private:
        void DrawEntityNode(GameObject *entity);
        static std::shared_ptr<Scene> s_activeScene;
        static GameObject *s_selectedEntity;
    };
}

#endif