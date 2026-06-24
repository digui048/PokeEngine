#include "Poke/Core/Application.h"

int main()
{
    auto app = new Poke::Application;
    app->Run();
    delete app;

    return 0;
}