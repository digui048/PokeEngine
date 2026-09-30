#include "AssetManager.h"
#include "Poke/Core/Log.h"

using namespace Poke;

AssetManagerBase *AssetManager::s_activeManager = nullptr;

void AssetManager::SetActive(AssetManagerBase *manager)
{
    if (!manager)
    {
        POKE_CORE_ERROR("[AssetManager] Trying to set a null asset manager");
        return;
    }

    s_activeManager = manager;

    POKE_CORE_INFO("[AssetManager] Active asset manager changes");
}

void AssetManager::Shutdown()
{
    s_activeManager = nullptr;
    POKE_CORE_INFO("[AssetManager] Shutdown");
}

AssetManagerBase *AssetManager::GetActive()
{
    return s_activeManager;
}

bool AssetManager::IsAssetHandleValid(AssetHandle handle)
{
    AssetManagerBase *manager = GetActive();

    if (!manager)
        return false;

    return manager->IsAssetHandleValid(handle);
}

bool AssetManager::IsAssetLoaded(AssetHandle handle)
{
    AssetManagerBase *manager = GetActive();

    if (!manager)
        return false;

    return manager->IsAssetLoaded(handle);
}

AssetType AssetManager::GetAssetType(AssetHandle handle)
{
    AssetManagerBase *manager = GetActive();

    if (!manager)
        return AssetType::None;

    return manager->GetAssetType(handle);
}
