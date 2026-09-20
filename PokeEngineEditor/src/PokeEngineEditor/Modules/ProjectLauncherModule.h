#ifndef PROJECT_LAUNCHER_MODULE
#define PROJECT_LAUNCHER_MODULE

#include "Poke/Core/Module.h"
#include <string>
#include <vector>

namespace Poke
{
    struct RecentProject
    {
        std::string name;
        std::string path;
    };

    class ProjectLauncherModule : public Module
    {
    public:
        ProjectLauncherModule();
        ~ProjectLauncherModule() override;

        void OnInit() override;
        void OnUpdate(float dt) override {}
        void OnRender(VkCommandBuffer cmd) override {}
        void OnImGuiRender() override;
        void OnShutdown() override {}

        void CreateNewProject(const std::string &path);
        void OpenExistingProject(const std::string &path);

    private:
        void LoadRecentProjects();
        void SaveRecentProjects();
        void AddToRecentProjects(const std::string &name, const std::string &path);
        void RemoveFromRecentProjects(size_t index);

    private:
        std::vector<RecentProject> m_recentProjects;
    };
}

#endif