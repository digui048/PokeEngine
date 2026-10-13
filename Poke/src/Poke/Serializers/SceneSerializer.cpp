#include "SceneSerializer.h"
#include "Poke/Core/Log.h"
#include "Poke/Utils/Json.h"
#include "Poke/Resources/AssetManager.h"
#include "Poke/Scene/Components/TransformComponent.h"
#include "Poke/Scene/Components/MeshRendererComponent.h"

using namespace Poke;

bool SceneSerializer::SerializeToLibrary(const AssetMetaData &metadata, const std::shared_ptr<Asset> &asset, const std::filesystem::path &binaryPath) const
{
    if (!asset)
    {
        POKE_CORE_ERROR("[SceneSerializer] Can not serialize null asset for handle {0}", static_cast<uint64_t>(metadata.handle));
        return false;
    }

    auto scene = std::static_pointer_cast<Scene>(asset);

    JsonNode root;
    nlohmann::json sceneJson;
    sceneJson["Handle"] = static_cast<uint64_t>(scene->GetHandle());

    nlohmann::json gameObjectsArray = nlohmann::json::array();
    if (scene->GetRoot())
    {
        for (const auto &child : scene->GetRoot()->GetChildren())
        {
            nlohmann::json objJson;
            SerializeGameObject(objJson, child.get());
            gameObjectsArray.push_back(objJson);
        }
    }
    sceneJson["GameObjects"] = gameObjectsArray;
    root.GetInternal()["Scene"] = sceneJson;

    std::filesystem::create_directories(binaryPath.parent_path());
    if (Json::SaveToFile(root, binaryPath))
    {
        POKE_CORE_INFO("[SceneSerializer] Scene serialized successfully to: {0}", binaryPath.string());
        return true;
    }

    POKE_CORE_ERROR("[SceneSerializer] Failed to open save scene to: {0}", binaryPath.string());
    return false;
}

std::shared_ptr<Asset> SceneSerializer::DeserializeFromLibrary(const AssetMetaData &metadata, const std::filesystem::path &binaryPath) const
{
    JsonNode root = Json::LoadFromFile(binaryPath);
    if (!root.HasKey("Scene"))
    {
        POKE_CORE_ERROR("[SceneSerializer] Failed to find scene key in: {0}", binaryPath.string());
        return nullptr;
    }

    auto scene = std::make_shared<Scene>(metadata.handle);
    scene->OnInit();

    const auto &sceneJson = root.GetInternal()["Scene"];
    if (sceneJson.contains("GameObjects") && sceneJson["GameObjects"].is_array())
    {
        for (const auto &objJson : sceneJson["GameObjects"])
            DeserializeGameObject(objJson, scene.get(), scene->GetRoot());
    }

    POKE_CORE_INFO("[SceneSerializer] Scene deserialized successfully from: {0}", binaryPath.string());
    return scene;
}

void SceneSerializer::SerializeGameObject(nlohmann::json &json, GameObject *gameObject) const
{
    if (!gameObject)
        return;

    json["UUID"] = static_cast<uint64_t>(gameObject->GetUUID());
    json["Name"] = gameObject->GetName();
    json["Active"] = gameObject->IsActive();
    json["ParentUUID"] = static_cast<uint64_t>(gameObject->GetParent()->GetUUID());

    nlohmann::json componentsJson = nlohmann::json::object();

    if (auto *transform = gameObject->GetComponent<TransformComponent>())
    {
        nlohmann::json transformJson;
        auto pos = transform->GetLocalTranslation();
        auto rot = transform->GetLocalRotationEuler();
        auto scale = transform->GetLocalScale();

        transformJson["Position"] = {pos.x, pos.y, pos.z};
        transformJson["Rotation"] = {rot.x, rot.y, rot.z};
        transformJson["Scale"] = {scale.x, scale.y, scale.z};

        componentsJson["Transform"] = transformJson;
    }

    if (auto *meshRenderer = gameObject->GetComponent<MeshRendererComponent>())
    {
        nlohmann::json meshRendererJson;
        meshRendererJson["MeshHandle"] = meshRenderer->GetMesh() ? static_cast<uint64_t>(meshRenderer->GetMesh()->GetHandle()) : 0;
        meshRendererJson["MaterialHandle"] = meshRenderer->GetMaterial() ? static_cast<uint64_t>(meshRenderer->GetMaterial()->GetHandle()) : 0;

        componentsJson["MeshRenderer"] = meshRendererJson;
    }

    json["Components"] = componentsJson;

    nlohmann::json childrenArray = nlohmann::json::array();
    for (const auto &child : gameObject->GetChildren())
    {
        nlohmann::json childJson;
        SerializeGameObject(childJson, child.get());
        childrenArray.push_back(childJson);
    }
    json["Children"] = childrenArray;
}

void SceneSerializer::DeserializeGameObject(const nlohmann::json &json, Scene *scene, GameObject *parent) const
{
    uint64_t uuidVal = json.value("UUID", 0);
    std::string name = json.value("Name", "GameObject");
    bool active = json.value("Active", true);

    auto gameObject = std::make_unique<GameObject>(UUID(uuidVal), name, parent);
    gameObject->SetActive(active);

    if (json.contains("Components"))
    {
        const auto &componentsJson = json["Components"];

        if (componentsJson.contains("Transform"))
        {
            const auto &transformJson = componentsJson["Transform"];
            if (auto *transform = gameObject->GetComponent<TransformComponent>())
            {
                if (transformJson.contains("Position") && transformJson["Position"].is_array())
                {
                    glm::vec3 pos(transformJson["Position"][0], transformJson["Position"][1], transformJson["Position"][2]);
                    transform->SetLocalTranslation(pos);
                }
                if (transformJson.contains("Rotation") && transformJson["Rotation"].is_array())
                {
                    glm::vec3 rot(transformJson["Rotation"][0], transformJson["Rotation"][1], transformJson["Rotation"][2]);
                    transform->SetLocalRotationEuler(rot);
                }
                if (transformJson.contains("Scale") && transformJson["Scale"].is_array())
                {
                    glm::vec3 scale(transformJson["Scale"][0], transformJson["Scale"][1], transformJson["Scale"][2]);
                    transform->SetLocalScale(scale);
                }
            }
        }

        if (componentsJson.contains("MeshRenderer"))
        {
            const auto &meshRendererJson = componentsJson["MeshRenderer"];
            uint64_t meshHandleVal = meshRendererJson.value("MeshHandle", uint64_t(0));
            uint64_t materialHandleVal = meshRendererJson.value("MaterialHandle", uint64_t(0));

            std::shared_ptr<Mesh> mesh = meshHandleVal != 0 ? AssetManager::GetAsset<Mesh>(meshHandleVal) : nullptr;
            std::shared_ptr<Material> material = materialHandleVal != 0 ? AssetManager::GetAsset<Material>(materialHandleVal) : nullptr;

            gameObject->AddComponent<MeshRendererComponent>(mesh, material);
        }
    }

    GameObject *gameObjectPtr = gameObject.get();
    parent->AddChild(std::move(gameObject));

    if (json.contains("Children") && json["Children"].is_array())
    {
        for (const auto &childJson : json["Children"])
            DeserializeGameObject(childJson, scene, gameObjectPtr);
    }
}
