#ifndef MAINMENU_BAR_INTERFACE_H
#define MAINMENU_BAR_INTERFACE_H

#include "EditorInterface.h"

namespace Poke
{
    class MainMenuBarInterface : public EditorInterface
    {
        public:
            MainMenuBarInterface() : EditorInterface("MainMenuBar") {}

            void OnInit() override;
            void OnImGuiRender() override;
    };
}

#endif