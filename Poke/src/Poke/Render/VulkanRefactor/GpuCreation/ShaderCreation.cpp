#include "ShaderCreation.h"

using namespace Poke;

ShaderStateCreation &ShaderStateCreation::Reset()
{
    stagesCount = 0;
    return *this;
}

ShaderStateCreation &ShaderStateCreation::AddStage(const char *code, uint32_t codeSize, VkShaderStageFlagBits type)
{
    stages[stagesCount].code = code;
    stages[stagesCount].codeSize = codeSize;
    stages[stagesCount].stage = type;
    ++stagesCount;

    return *this;
}

ShaderStateCreation &ShaderStateCreation::SetSPVInput(bool value)
{
    spvInput = value;
    return *this;
}

ShaderStateCreation &ShaderStateCreation::SetName(const char *name)
{
    this->name = name;
    return *this;
}
