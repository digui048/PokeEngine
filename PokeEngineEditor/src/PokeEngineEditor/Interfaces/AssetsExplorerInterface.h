#ifndef ASSETS_EXPLORER_INTERFACE
#define ASSETS_EXPLORER_INTERFACE

#include "EditorInterface.h"
#include "Poke/Resources/Texture.h"

#include <filesystem>

namespace Poke
{
    class AssetsExplorerInterface : public EditorInterface
    {
    public:
        AssetsExplorerInterface(const std::filesystem::path &path);
        ~AssetsExplorerInterface() override {}

        void OnInit() override;
        void OnImGuiRender() override;

        static std::filesystem::path GetSelectedFile() { return s_selectedFile; }
        static void SetSelectedFile(const std::filesystem::path &file) { s_selectedFile = file; }

        void OnFileDropped(const char* path, float x, float y);

    private:
        std::filesystem::path m_assetsDirectory;
        std::filesystem::path m_currentDirectory;

        static std::filesystem::path s_selectedFile;

        std::shared_ptr<Texture> m_directoryIcon;
        std::shared_ptr<Texture> m_fileIcon;
    };
}

#endif