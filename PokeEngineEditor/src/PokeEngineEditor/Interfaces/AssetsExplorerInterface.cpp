#include "AssetsExplorerInterface.h"
#include "Poke/Core/Log.h"

#include "Poke/Importers/TextureImporter.h"

#include <imgui.h>

using namespace Poke;

std::filesystem::path AssetsExplorerInterface::s_selectedFile = "";

AssetsExplorerInterface::AssetsExplorerInterface(const std::filesystem::path &path, EditorAssetManager *assetManager)
    : EditorInterface("Assets Explorer"), m_assetManager(assetManager)
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
    float arrowSize = ImGui::GetFrameHeight();

    float cellSize = thumbnailSize + padding + arrowSize;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = static_cast<int>(panelWidth / cellSize);
    if (columnCount < 1)
        columnCount = 1;

    ImGui::Columns(columnCount, 0, false);

    if (std::filesystem::exists(m_currentDirectory))
    {
        std::filesystem::path pathToDelete = "";
        bool isDirectoryToDelete = false;
        AssetHandle handleToImport;

        for (auto &directoryEntry : std::filesystem::directory_iterator(m_currentDirectory))
        {
            const auto &path = directoryEntry.path();
            std::string filenameString = path.filename().string();
            bool isDirectory = directoryEntry.is_directory();

            AssetHandle assetHandle = m_assetManager->GetAssetHandle(path);
            if (assetHandle)
            {
                const AssetMetaData &metadata = m_assetManager->GetMetaData(assetHandle);

                if (metadata.IsSubAsset())
                    continue;
            }

            if (!isDirectory && m_assetManager->IsAssetFile(path) || path.filename() == "AssetRegistry.json")
                continue;

            ImGui::PushID(filenameString.c_str());

            bool isModel = assetHandle && m_assetManager->GetAssetType(assetHandle) == AssetType::Model;
            bool isExpanded = isModel && m_expandedModels.contains(assetHandle);

            ImGui::BeginGroup();
            Texture *icon = directoryEntry.is_directory() ? m_directoryIcon.get() : m_fileIcon.get();
            ImTextureID textureID = icon ? icon->GetImGuiTextureID() : (ImTextureID)0;

            bool isSelected = (s_selectedFile == path);
            float imageStartY = ImGui::GetCursorPosY();

            if (isSelected)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 0.5f));
            else
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

            if (ImGui::ImageButton("##icon", textureID, {thumbnailSize, thumbnailSize}, {0, 0}, {1, 1}))
            {
                s_selectedFile = path;
            }

            ImGui::PopStyleColor();

            if (isModel)
            {
                ImGui::SameLine(0.0f, 4.0f);

                const float arrowSize = ImGui::GetFrameHeight();

                ImGui::SetCursorPosY(imageStartY + (thumbnailSize - arrowSize) * 0.5f);

                ImGui::PushID("arrow");
                if (ImGui::ArrowButton("##expand", isExpanded ? ImGuiDir_Down : ImGuiDir_Right))
                {
                    if (isExpanded)
                        m_expandedModels.erase(assetHandle);
                    else
                    {
                        m_expandedModels.insert(assetHandle);
                        std::shared_ptr<Asset> model = m_assetManager->GetAsset(assetHandle);

                        if (!model)
                            m_expandedModels.erase(assetHandle);
                    }
                }

                ImGui::PopID();
            }

            ImGui::SetCursorPosY(imageStartY + thumbnailSize + ImGui::GetStyle().ItemSpacing.y);

            ImGui::TextWrapped("%s", filenameString.c_str());
            ImGui::EndGroup();

            if (isModel && isExpanded)
            {
                std::vector<AssetHandle> meshes = m_assetManager->GetModelMeshes(assetHandle);
                if (!meshes.empty())
                {
                    ImDrawList *drawList = ImGui::GetWindowDrawList();
                    drawList->ChannelsSplit(2);
                    drawList->ChannelsSetCurrent(1);

                    std::vector<std::pair<ImVec2, ImVec2>> rowRects;

                    for (AssetHandle meshHandle : meshes)
                    {
                        ImGui::NextColumn();

                        ImVec2 itemMin = ImGui::GetCursorScreenPos();

                        const AssetMetaData &meshMetadata = m_assetManager->GetMetaData(meshHandle);
                        std::string meshFilename = meshMetadata.filePath.filename().string();

                        ImGui::PushID(static_cast<uint64_t>(meshHandle));
                        ImGui::BeginGroup();

                        Texture *meshIcon = m_fileIcon.get();
                        ImTextureID meshTextureID = meshIcon ? meshIcon->GetImGuiTextureID() : (ImTextureID)0;

                        bool meshSelected = s_selectedFile == meshMetadata.filePath;

                        if (meshSelected)
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 0.5f));
                        else
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

                        if (ImGui::ImageButton("##icon", meshTextureID, {thumbnailSize, thumbnailSize}, {0, 0}, {1, 1}))
                        {
                            s_selectedFile = meshMetadata.filePath;
                        }

                        ImGui::PopStyleColor();
                        ImGui::TextWrapped("%s", meshFilename.c_str());

                        ImGui::EndGroup();

                        ImVec2 itemMax = ImGui::GetItemRectMax();

                        if (rowRects.empty() || std::abs(itemMin.y - rowRects.back().first.y) > 8.0f)
                            rowRects.push_back({itemMin, itemMax});
                        else
                        {
                            std::pair<ImVec2,ImVec2>& current = rowRects.back();
                            current.first.x = std::min(current.first.x, itemMin.x);
                            current.first.y = std::min(current.first.y, itemMin.y);
                            current.second.x = std::max(current.second.x, itemMax.x);
                            current.second.y = std::max(current.second.y, itemMax.y);
                        }

                        ImGui::PopID();
                    }

                    drawList->ChannelsSetCurrent(0);

                    ImVec2 winMin = ImGui::GetWindowPos();
                    ImVec2 winMax = ImVec2(winMin.x + ImGui::GetWindowSize().x, winMin.y + ImGui::GetWindowSize().y);
                    drawList->PushClipRect(winMin, winMax, false);

                    const float rectPadding = 4.0f;
                    for (const auto& rect : rowRects)
                    {
                        ImVec2 paddedMin = ImVec2(rect.first.x - rectPadding, rect.first.y - rectPadding);
                        ImVec2 paddedMax = ImVec2(rect.second.x + rectPadding, rect.second.y + rectPadding);
                        
                        drawList->AddRectFilled(paddedMin, paddedMax, IM_COL32(40, 100, 180, 50), 4.0f);
                        drawList->AddRect(paddedMin, paddedMax, IM_COL32(70, 150, 240, 220), 4.0f, 0, 1.5f);
                    }
                    
                    drawList->PopClipRect();
                    drawList->ChannelsMerge();
                }
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (isDirectory)
                {
                    m_currentDirectory /= path.filename();
                }
            }

            if (ImGui::BeginPopupContextItem("##context"))
            {
                s_selectedFile = path;
                AssetHandle handle = m_assetManager->GetAssetHandle(path);

                if (handle)
                {
                    if (ImGui::MenuItem("Import"))
                    {
                        handleToImport = handle;
                    }
                    ImGui::Separator();
                }

                if (ImGui::MenuItem("Delete"))
                {
                    pathToDelete = path;
                    isDirectoryToDelete = isDirectory;
                }
                ImGui::EndPopup();
            }

            if (isSelected && ImGui::IsKeyPressed(ImGuiKey_Delete))
            {
                pathToDelete = path;
                isDirectoryToDelete = isDirectory;
            }

            ImGui::NextColumn();
            ImGui::PopID();
        }

        if (!pathToDelete.empty())
        {
            if (isDirectoryToDelete)
            {
                RemoveDirectory(pathToDelete.string().c_str());
            }
            else
            {
                RemoveFile(pathToDelete.string().c_str());
            }

            if (s_selectedFile == pathToDelete)
            {
                s_selectedFile.clear();
            }
        }

        if (handleToImport)
        {
            m_assetManager->GetAsset(handleToImport);
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
        POKE_CORE_ERROR("File already exists: {0}", destinationPath.string());
        return;
    }

    try
    {
        if (std::filesystem::is_directory(sourcePath))
        {
            std::filesystem::copy(sourcePath, destinationPath, std::filesystem::copy_options::recursive);
        }
        else if (std::filesystem::is_regular_file(sourcePath))
        {
            std::filesystem::copy_file(sourcePath, destinationPath);
            if (m_assetManager->IsAssetFile(destinationPath))
            {
                AssetHandle handle = m_assetManager->RegisterAsset(destinationPath);
                m_assetManager->SerializeAssetRegistry();
            }
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        POKE_CORE_ERROR("Failed to copy file: {0}", e.what());
    }
}

void AssetsExplorerInterface::RemoveDirectory(const char *path)
{
    if (!path)
        return;

    std::filesystem::path directoryPath(path);
    if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath))
        return;

    for (const auto &entry : std::filesystem::recursive_directory_iterator(directoryPath))
    {
        if (!entry.is_regular_file())
            continue;

        AssetHandle handle = m_assetManager->GetAssetHandle(entry.path());
        if (handle)
            m_assetManager->RemoveAsset(handle);
    }

    std::error_code ec;
    std::filesystem::remove_all(path, ec);
    if (ec)
    {
        POKE_CORE_ERROR("Failed to remove directory '{0}': {1}", path, ec.message());
    }
}

void AssetsExplorerInterface::RemoveFile(const char *path)
{
    if (!path)
        return;

    std::filesystem::path filePath(path);

    AssetHandle handle = m_assetManager->GetAssetHandle(filePath);
    if (handle)
        m_assetManager->RemoveAsset(handle);

    std::error_code ec;
    if (std::filesystem::exists(path, ec) && std::filesystem::is_regular_file(path, ec))
    {
        std::filesystem::remove(path, ec);
        if (ec)
        {
            POKE_CORE_ERROR("Failed to remove file '{0}': {1}", path, ec.message());
        }
    }
}
