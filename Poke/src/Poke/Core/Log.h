#ifndef LOG_H
#define LOG_H

#include <memory>
#include <spdlog/spdlog.h>

namespace Poke
{
    class Log
    {
    public:
        static void Init();

        static std::shared_ptr<spdlog::logger> &GetCoreLogger();
        static std::shared_ptr<spdlog::logger> &GetClientLogger();

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}

// core logger
#define POKE_CORE_TRACE(...)    ::Poke::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define POKE_CORE_INFO(...)     ::Poke::Log::GetCoreLogger()->info(__VA_ARGS__)
#define POKE_CORE_WARN(...)     ::Poke::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define POKE_CORE_ERROR(...)    ::Poke::Log::GetCoreLogger()->error(__VA_ARGS__)
#define POKE_CORE_CRITICAL(...) ::Poke::Log::GetCoreLogger()->critical(__VA_ARGS__)

// client logger
#define POKE_TRACE(...)         ::Poke::Log::GetClientLogger()->trace(__VA_ARGS__)
#define POKE_INFO(...)          ::Poke::Log::GetClientLogger()->info(__VA_ARGS__)
#define POKE_WARN(...)          ::Poke::Log::GetClientLogger()->warn(__VA_ARGS__)
#define POKE_ERROR(...)         ::Poke::Log::GetClientLogger()->error(__VA_ARGS__)
#define POKE_CRITICAL(...)      ::Poke::Log::GetClientLogger()->critical(__VA_ARGS__)

#endif