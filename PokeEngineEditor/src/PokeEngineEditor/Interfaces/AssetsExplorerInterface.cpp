#include "AssetsExplorerInterface.h"
#include "Poke/Core/Log.h"

#include "Poke/Importers/TextureImporter.h"

#include <imgui.h>

using namespace Poke;

std::filesystem::path AssetsExplorerInterface::s_selectedFile = "";

AssetsExplorerInterface::AssetsExplorerInterface(const std::filesystem::path &path)
    : EditorInterface("Assets Explorer")
{
    m_assetsDirectory = path / "Assets";

    if (!std::filesystem::exists(m_assetsDirectory))
    {
        std::filesystem::create_directories(m_assetsDirectory);
    }

    m_currentDirectory = m_assetsDirectory;

    m_directoryIcon = TextureImporter::LoadTexture("PokeEngineEditor/assets/icons/directoryIcon.png");
    m_fileIcon = TextureImporter::LoadTexture("PokeEngineEditor/assets/icons/fileIcon.png");
}

void AssetsExplorerInterface::OnInit()
{
}

void AssetsExplorerInterface::OnImGuiRender()
{
    if (!m_isOpen)
        return;

    ImGui::Begin(m_name.c_str(), &m_isOpen);

    m_windowPos = ImGui::GetWindowPos();
    m_windowSize = ImGui::GetWindowSize();
    m_isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

    if (m_currentDirectory != m_assetsDirectory)
    {
        if (ImGui::Button("<-"))
        {
            m_currentDirectory = m_currentDirectory.parent_path();
        }
        ImGui::SameLine();
    }

    std::string relativePath = std::filesystem::relative(m_currentDirectory, m_assetsDirectory).string();
    ImGui::Text("Assets/%s", relativePath == "." ? "" : relativePath.c_str());
    ImGui::Separator();

    static float padding = 16.0f;
    static float thumbnailSize = 50.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = static_cast<int>(panelWidth / cellSize);
    if (columnCount < 1)
        columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    if (std::filesystem::exists(m_currentDirectory))
    {
        for (auto &directoryEntry : std::filesystem::directory_iterator(m_currentDirectory))
        {
            const auto &path = directoryEntry.path();
            std::string filenameString = path.filename().string();

            ImGui::PushID(filenameString.c_str());
            Texture *icon = directoryEntry.is_directory() ? m_directoryIcon.get() : m_fileIcon.get();
            ImTextureID textureID = icon ? icon->GetImGuiTextureID() : (ImTextureID)0;
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            if (ImGui::ImageButton("##icon", textureID, {thumbnailSize, thumbnailSize}, {0, 0}, {1, 1}))
            {
                s_selectedFile = path;
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (directoryEntry.is_directory())
                {
                    m_currentDirectory /= path.filename();
                }
            }

            ImGui::PopStyleColor();

            ImGui::TextWrapped("%s", filenameString.c_str());
            ImGui::NextColumn();
            ImGui::PopID();
        }
    }

    ImGui::Columns(1);

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
    {
        s_selectedFile.clear();
    }

    ImGui::End();
}

void AssetsExplorerInterface::OnFileDropped(const char *path, float x, float y)
{
    if (!IsInside(x, y) || !path)
        return;

    std::filesystem::path sourcePath(path);
    std::filesystem::path destinationPath = m_currentDirectory / sourcePath.filename();
    
    if (std::filesystem::exists(destinationPath))
    {
        POKE_ERROR("File already exists: {0}", destinationPath.string());
        return;
    }

    try
    {
        std::filesystem::copy_file(sourcePath, destinationPath);
    }
    catch(const std::filesystem::filesystem_error &e)
    {
        POKE_ERROR("Failed to copy file: {0}", e.what());
    }
}
