#include "DescriptorSetCreation.h"

using namespace Poke;

DescriptorSetLayoutCreation &DescriptorSetLayoutCreation::Reset()
{
    bindingCount = 0;
    setIndex = 0;

    return *this;
}

DescriptorSetLayoutCreation &DescriptorSetLayoutCreation::AddBinding(const Binding &binding)
{
    bindings[bindingCount++] = binding;
    return *this;
}

DescriptorSetLayoutCreation &DescriptorSetLayoutCreation::SetSetIndex(uint32_t index)
{
    setIndex = index;
    return *this;
}

DescriptorSetLayoutCreation &DescriptorSetLayoutCreation::SetName(const char *name)
{
    this->name = name;
    return *this;
}

DescriptorSetCreation &DescriptorSetCreation::Reset()
{
    resourceCount = 0;
    return *this;
}

DescriptorSetCreation &DescriptorSetCreation::SetLayout(DescriptorSetLayoutHandle layout)
{
    this->layout = layout;
    return *this;
}

DescriptorSetCreation &DescriptorSetCreation::Texture(TextureHandle texture, uint16_t binding)
{
    resources[resourceCount] = texture.index;
    samplers[resourceCount] = k_invalidSampler;
    bindings[resourceCount++] = binding;

    return *this;
}

DescriptorSetCreation &DescriptorSetCreation::Buffer(BufferHandle buffer, uint16_t binding)
{
    resources[resourceCount] = buffer.index;
    samplers[resourceCount] = k_invalidSampler;
    bindings[resourceCount++] = binding;

    return *this;
}

DescriptorSetCreation &DescriptorSetCreation::TextureSampler(TextureHandle texture, SamplerHandle sampler, uint16_t binding)
{
    resources[resourceCount] = texture.index;
    samplers[resourceCount] = sampler;
    bindings[resourceCount++] = binding;

    return *this;
}

DescriptorSetCreation &DescriptorSetCreation::SetName(const char *name)
{
    this->name = name;
    return *this;
}
