#ifndef TIME_H
#define TIME_H

#include <cstdint>

namespace Poke
{
    class Time
    {
    public:
        static void Init();
        static void Update();

        static float DeltaTime();
        static double TotalTime();

        static float FPS();
        static uint64_t FrameCount();

    private:
        static uint64_t s_LastCounter;
        static double s_DeltaTime;
        static double s_TotalTime;
        static uint64_t s_FrameCount;
    };
}

#endif