#ifndef HIERARCHY_INTERFACE_H
#define HIERARCHY_INTERFACE_H

#include "EditorInterface.h"

namespace Poke
{
    class Scene;
    class GameObject;

    class HierarchyInterface : public EditorInterface
    {
    public:
        HierarchyInterface(Scene *scene) : EditorInterface("Hierarchy"), m_scene(scene) {}

        void OnInit() override;
        void OnImGuiRender() override;

        static GameObject *GetSelectedEntity() { return s_selectedEntity; }
        static void SetSelectedEntity(GameObject *entity) { s_selectedEntity = entity; }

    private:
        void DrawEntityNode(GameObject *entity);
        Scene *m_scene;
        static GameObject *s_selectedEntity;
    };
}

#endif