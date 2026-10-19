#include "TextureCreation.h"

using namespace Poke;

TextureCreation& TextureCreation::SetSize(uint16_t width, uint16_t height, uint16_t depth)
{
    this->width = width;
    this->height = height;
    this->depth = depth;

    return *this;
}

TextureCreation& TextureCreation::SetFlags(uint8_t mipmaps, uint8_t flags)
{
    this->mipmaps = mipmaps;
    this->flags = flags;

    return *this;
}

TextureCreation& TextureCreation::SetFormatType(VkFormat format, TextureType::Enum type)
{
    this->format = format;
    this->type = type;

    return *this;
}

TextureCreation& TextureCreation::SetData(void *data)
{
    this->initialData = data;
    return *this;
}

TextureCreation& TextureCreation::SetName(const char *name)
{
    this->name = name;
    return *this;
}
