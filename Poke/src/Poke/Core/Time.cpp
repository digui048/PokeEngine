#include "Time.h"

#include <SDL3/SDL_timer.h>

using namespace Poke;

uint64_t Time::s_LastCounter = 0;
double Time::s_DeltaTime = 0.0;
double Time::s_TotalTime = 0.0;
uint64_t Time::s_FrameCount = 0;

void Time::Init()
{
    s_LastCounter = SDL_GetPerformanceCounter();
}

void Time::Update()
{
    uint64_t current = SDL_GetPerformanceCounter();
    uint64_t frequency = SDL_GetPerformanceFrequency();

    s_DeltaTime = static_cast<double>(current - s_LastCounter) /
                  static_cast<double>(frequency);

    s_TotalTime += s_DeltaTime;
    s_FrameCount++;

    s_LastCounter = current;
}

float Time::DeltaTime()
{
    return static_cast<float>(s_DeltaTime);
}

double Time::TotalTime()
{
    return s_TotalTime;
}

float Time::FPS()
{
    return s_DeltaTime > 0.0 ? static_cast<float>(1.0 / s_DeltaTime) : 0.0f;
}

uint64_t Time::FrameCount()
{
    return s_FrameCount;
}