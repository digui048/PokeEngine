#include "TextureImporter.h"
#include "Poke/Core/Log.h"

#include <IL/il.h>

using namespace Poke;

std::shared_ptr<Texture> TextureImporter::LoadTexture(const std::string &filepath)
{
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    if (!ilLoadImage(filepath.c_str()))
    {
        ilDeleteImages(1, &imageID);
        POKE_CORE_ERROR("Failed to load texture image with path: {0}", filepath);
        return nullptr;
    }

    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    uint32_t width = ilGetInteger(IL_IMAGE_WIDTH);
    uint32_t height = ilGetInteger(IL_IMAGE_HEIGHT);
    ILubyte *pixels = ilGetData();

    if (!pixels)
    {
        ilDeleteImages(1, &imageID);
        POKE_CORE_ERROR("Failed to get pixel data");
        return nullptr;
    }

    auto texture = std::make_shared<Texture>(pixels, width, height);

    ilDeleteImages(1, &imageID);

    return texture;
}