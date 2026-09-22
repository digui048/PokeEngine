#ifndef MODULE_H
#define MODULE_H

#include <vulkan/vulkan.h>

namespace Poke
{
    class Module
    {
    public:
        virtual ~Module() = default;

        virtual void OnInit() {}
        virtual void OnUpdate(float dt) {}
        virtual void OnRender(VkCommandBuffer cmd) {}
        virtual void OnImGuiRender() {}
        virtual void OnShutdown() {}

    public:
        virtual void OnFileDropped(const char *path, float x, float y) {}
    };
}

#endif