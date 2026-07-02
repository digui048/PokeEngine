#ifndef MODULE_H
#define MODULE_H

namespace Poke
{
    class Module
    {
        public:
            virtual ~Module() = default;

            virtual void OnInit() {}
            virtual void OnUpdate(float dt) {}
            virtual void OnImGuiRender() {}
            virtual void OnShutdown() {}
    };
}

#endif