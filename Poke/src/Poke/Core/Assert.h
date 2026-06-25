#ifndef ASSERT_H
#define ASSERT_H

#include "Log.h"

#include <cstdlib>

#if defined(_WIN32)
#define POKE_DEBUGBREAK() __debugbreak()
#elif defined(__linux__)
#include <signal.h>
#define POKE_DEBUGBREAK() raise(SIGTRAP)
#else
#define POKE_DEBUGBREAK() ((void)0)
#endif

#define POKE_ASSERT(condition, ...)                                                 \
    do                                                                              \
    {                                                                               \
        if (!(condition))                                                           \
        {                                                                           \
            POKE_CORE_CRITICAL(__VA_ARGS__);                                        \
            POKE_CORE_CRITICAL("Assertion failed at {0}:{1}", __FILE__, __LINE__);  \
            POKE_DEBUGBREAK();                                                      \
            std::abort();                                                           \
        }                                                                           \
    } while (0)

#endif