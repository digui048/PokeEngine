#ifndef PROJECT_LAUNCHER_MODULE
#define PROJECT_LAUNCHER_MODULE

#include "Poke/Core/Module.h"
#include <string>

namespace Poke
{
    class ProjectLauncherModule : public Module
    {
    public:
        ProjectLauncherModule();
        ~ProjectLauncherModule() override;

        void OnInit() override {}
        void OnUpdate(float dt) override {}
        void OnRender(VkCommandBuffer cmd) override {}
        void OnImGuiRender() override;
        void OnShutdown() override {}

        void CreateNewProject(const std::string &path);
        void OpenExistingProject(const std::string &path);
    };
}

#endif