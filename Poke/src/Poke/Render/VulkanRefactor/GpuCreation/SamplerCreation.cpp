#include "SamplerCreation.h"

using namespace Poke;

SamplerCreation &SamplerCreation::SetMinMagMip(VkFilter min, VkFilter mag, VkSamplerMipmapMode mip)
{
    minFilter = min;
    magFilter = mag;
    mipmapMode = mip;

    return *this;
}

SamplerCreation &SamplerCreation::SetAddressModeU(VkSamplerAddressMode u)
{
    addressU = u;
    return *this;
}

SamplerCreation &SamplerCreation::SetAddressModeUV(VkSamplerAddressMode u, VkSamplerAddressMode v)
{
    addressU = u;
    addressV = v;

    return *this;
}

SamplerCreation &SamplerCreation::SetAddressModeUVW(VkSamplerAddressMode u, VkSamplerAddressMode v, VkSamplerAddressMode w)
{
    addressU = u;
    addressV = v;
    addressW = w;

    return *this;
}

SamplerCreation &SamplerCreation::SetName(const char *name)
{
    this->name = name;
    return *this;
}
