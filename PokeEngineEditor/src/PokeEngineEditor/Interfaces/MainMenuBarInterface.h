#ifndef MAINMENU_BAR_INTERFACE_H
#define MAINMENU_BAR_INTERFACE_H

#include "EditorInterface.h"
#include <filesystem>

namespace Poke
{
    class EditorAssetManager;
    class Scene;

    class MainMenuBarInterface : public EditorInterface
    {
        public:
            MainMenuBarInterface(EditorAssetManager *assetManager, const std::filesystem::path &projectDir);
            ~MainMenuBarInterface() override = default;

            void OnInit() override;
            void OnImGuiRender() override;

        private:
            EditorAssetManager *m_assetManager;
            std::filesystem::path m_projectDir;
    };
}

#endif