#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace Poke;

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

void Log::Init()
{
    spdlog::set_pattern("%^[%T] %n: %v%$");

    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/PokeEngine.log", true);

    std::vector<spdlog::sink_ptr> coreSinks{consoleSink, fileSink};
    std::vector<spdlog::sink_ptr> clientSinks{consoleSink, fileSink};

    s_CoreLogger = std::make_shared<spdlog::logger>("POKE", coreSinks.begin(), coreSinks.end());
    spdlog::register_logger(s_CoreLogger);
    s_CoreLogger->set_level(spdlog::level::trace);
    s_CoreLogger->flush_on(spdlog::level::trace);

    s_ClientLogger = std::make_shared<spdlog::logger>("APP", clientSinks.begin(), clientSinks.end());
    spdlog::register_logger(s_ClientLogger);
    s_ClientLogger->set_level(spdlog::level::trace);
    s_ClientLogger->flush_on(spdlog::level::trace);

    s_CoreLogger->info("Logger initialized");
}

std::shared_ptr<spdlog::logger> &Log::GetCoreLogger()
{
    return s_CoreLogger;
}

std::shared_ptr<spdlog::logger> &Log::GetClientLogger()
{
    return s_ClientLogger;
}
