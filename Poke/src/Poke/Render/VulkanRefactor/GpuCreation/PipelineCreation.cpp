#include "PipelineCreation.h"

using namespace Poke;

PipelineCreation &PipelineCreation::AddDescriptorSetLayout(DescriptorSetLayoutHandle layout)
{
    descriptorSetLayouts[activeLayoutsCount++] = layout;
    return *this;
}

RenderPassOutput &PipelineCreation::GetRenderPassOutput()
{
    return renderPass;
}
