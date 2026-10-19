#include "SamplerCreation.h"

using namespace Poke;

SamplerCreation &SamplerCreation::SetMinMagMip(VkFilter min, VkFilter mag, VkSamplerMipmapMode mip)
{
    minFilter = min;
    magFilter = mag;
    mipFilter = mip;

    return *this;
}

SamplerCreation &SamplerCreation::SetAddressModeU(VkSamplerAddressMode u)
{
    adressU = u;
    return *this;
}

SamplerCreation &SamplerCreation::SetAddressModeUV(VkSamplerAddressMode u, VkSamplerAddressMode v)
{
    adressU = u;
    adressV = v;

    return *this;
}

SamplerCreation &SamplerCreation::SetAddressModeUVW(VkSamplerAddressMode u, VkSamplerAddressMode v, VkSamplerAddressMode w)
{
    adressU = u;
    adressV = v;
    adressW = w;

    return *this;
}

SamplerCreation &SamplerCreation::SetName(const char *name)
{
    this->name = name;
    return *this;
}
