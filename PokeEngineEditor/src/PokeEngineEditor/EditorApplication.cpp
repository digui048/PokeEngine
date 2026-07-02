#include "Poke/Core/Application.h"
#include "PokeEngineEditor/Modules/EditorModule.h"

using namespace Poke;

class EditorApplication : public Application
{
    public:
        EditorApplication() : Application() 
        {
            PushModule(std::make_shared<EditorModule>());
        }
};

int main()
{
    auto app = new EditorApplication();
    app->Run();
    delete app;

    return 0;
}