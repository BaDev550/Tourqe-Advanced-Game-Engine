#pragma once

#include <iostream>
#include "spdlog/spdlog.h"
#include "TAGE/Common/TEnums.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include "LogSink.h"
#include "Memory.h"
#include <format>

#if defined(_MSC_VER)
#define DEBUG_BREAK __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#define DEBUG_BREAK __builtin_trap()
#else
#include <csignal>
#define DEBUG_BREAK raise(SIGTRAP)
#endif
#define CHECK_LOGGER if(TAGE::Logger::GetCoreLogger() != nullptr)
#define CHECK_APP_LOGGER if(TAGE::Logger::GetClientLogger() != nullptr)

#define LOG_TRACE(...)    CHECK_LOGGER{TAGE::Logger::GetCoreLogger()->trace(__VA_ARGS__); }
#define LOG_INFO(...)     CHECK_LOGGER{TAGE::Logger::GetCoreLogger()->info(__VA_ARGS__); }
#define LOG_WARN(...)     CHECK_LOGGER{TAGE::Logger::GetCoreLogger()->warn(__VA_ARGS__); }
#define LOG_ERROR(...)    CHECK_LOGGER{TAGE::Logger::GetCoreLogger()->error(__VA_ARGS__); } 
#define LOG_CRITICAL(...) CHECK_LOGGER{TAGE::Logger::GetCoreLogger()->critical(__VA_ARGS__); }

#define APP_LOG_TRACE(...)    CHECK_APP_LOGGER{TAGE::Logger::GetClientLogger()->trace(__VA_ARGS__); }
#define APP_LOG_INFO(...)     CHECK_APP_LOGGER{TAGE::Logger::GetClientLogger()->info(__VA_ARGS__); }
#define APP_LOG_WARN(...)     CHECK_APP_LOGGER{TAGE::Logger::GetClientLogger()->warn(__VA_ARGS__); }
#define APP_LOG_ERROR(...)    CHECK_APP_LOGGER{TAGE::Logger::GetClientLogger()->error(__VA_ARGS__); } 
#define APP_LOG_CRITICAL(...) CHECK_APP_LOGGER{TAGE::Logger::GetClientLogger()->critical(__VA_ARGS__); }
#define ASSERT(x, ...) \
    if (!(x)) { \
        LOG_CRITICAL("ASSERTION FAILED: ({0})", #x); \
        LOG_CRITICAL(__VA_ARGS__); \
        DEBUG_BREAK; \
    }

#define ASSERT_NOMSG(x) \
    if (!(x)) { \
        LOG_CRITICAL("ASSERTION FAILED: ({0})", #x); \
        DEBUG_BREAK; \
    }

#define GET_MESSAGES() TAGE::Logger::GetSink()->GetMessages();
#define CLEAR_MESSAGES() TAGE::Logger::GetSink()->Clear();
namespace TAGE
{
    class Logger {
    public:
        static void init() {
            spdlog::set_pattern("%^[%n][%l] %T: %v%$");
            s_Sink = MEM::MakeRef<LogSink>();

            auto core_sink = MEM::MakeRef<spdlog::sinks::stdout_color_sink_mt>();
            s_CoreLogger = MEM::MakeRef<spdlog::logger>("CORE", spdlog::sinks_init_list{ core_sink, s_Sink });
            spdlog::register_logger(s_CoreLogger);
            s_CoreLogger->set_level(spdlog::level::trace);

            auto client_sink = MEM::MakeRef<spdlog::sinks::stdout_color_sink_mt>();
            s_ClientLogger = MEM::MakeRef<spdlog::logger>("APP", spdlog::sinks_init_list{ client_sink, s_Sink });
            spdlog::register_logger(s_ClientLogger);
            s_ClientLogger->set_level(spdlog::level::trace);
        }

        inline static MEM::Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        inline static MEM::Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
        inline static MEM::Ref<LogSink>& GetSink() { return s_Sink; }

    private:
        static MEM::Ref<spdlog::logger> s_CoreLogger;
        static MEM::Ref<spdlog::logger> s_ClientLogger;
        inline static MEM::Ref<LogSink> s_Sink;
    };
}