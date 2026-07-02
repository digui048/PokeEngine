#ifndef HIERARCHY_INTERFACE_H
#define HIERARCHY_INTERFACE_H

#include "EditorInterface.h"

namespace Poke
{
    class HierarchyInterface : public EditorInterface
    {
        public:
            HierarchyInterface() : EditorInterface("Hierarchy") {}

            void OnInit() override;
            void OnImGuiRender() override;
    };
}

#endif