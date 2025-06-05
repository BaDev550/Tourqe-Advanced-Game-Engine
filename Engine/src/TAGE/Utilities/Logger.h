#pragma once

#include <iostream>
#include "spdlog/spdlog.h"
#include "TAGE/Common/TEnums.h"
#include "spdlog/sinks/stdout_color_sinks.h"
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
#define CHECK_LOGGER if(TAGE::Logger::GetLogger() != nullptr)

#define LOG_TRACE(...)    CHECK_LOGGER{TAGE::Logger::GetLogger()->trace(__VA_ARGS__); }
#define LOG_INFO(...)     CHECK_LOGGER{TAGE::Logger::GetLogger()->info(__VA_ARGS__); }
#define LOG_WARN(...)     CHECK_LOGGER{TAGE::Logger::GetLogger()->warn(__VA_ARGS__); }
#define LOG_ERROR(...)    CHECK_LOGGER{TAGE::Logger::GetLogger()->error(__VA_ARGS__); } 
#define LOG_CRITICAL(...) CHECK_LOGGER{TAGE::Logger::GetLogger()->critical(__VA_ARGS__); }
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

namespace TAGE
{
    class Logger {
    public:
        static MEM::Ref<spdlog::logger> logger;
        static void init() {
            spdlog::set_pattern("%^[%n][%l] %T: %v%$");
            logger = spdlog::stdout_color_mt("CORE");
            logger->set_level(spdlog::level::trace);
        }
        static void Destroy() { logger = nullptr; }
        inline static MEM::Ref<spdlog::logger>& GetLogger() { return logger; }
    };
}