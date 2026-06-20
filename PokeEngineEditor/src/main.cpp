#include <iostream>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <glm/glm.hpp>

int main()
{
    std::cout << "--- 2.5D Engine Environment Verification ---" << std::endl;
    Uint64 now = SDL_GetPerformanceCounter();
    std::cout << now << std::endl;
    glm::vec4 vec = {1,2,3,4};
    std::cout << " aaaaaaaa" << std::endl; 
    return 0;
}