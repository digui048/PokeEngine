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
        HierarchyInterface(Scene* scene) : EditorInterface("Hierarchy"), m_scene(scene) {}

        void OnInit() override;
        void OnImGuiRender() override;

    private:
        void DrawEntityNode(GameObject *entity);
        Scene *m_scene;
    };
}

#endif