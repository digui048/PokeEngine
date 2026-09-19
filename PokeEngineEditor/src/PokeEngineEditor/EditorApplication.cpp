#include "Poke/Core/Application.h"
#include "PokeEngineEditor/Modules/ProjectLauncherModule.h"

using namespace Poke;

class EditorApplication : public Application
{
    public:
        EditorApplication() : Application() 
        {
            PushModule(std::make_shared<ProjectLauncherModule>());
        }
};

int main()
{
    auto app = new EditorApplication();
    app->Run();
    delete app;

    return 0;
}