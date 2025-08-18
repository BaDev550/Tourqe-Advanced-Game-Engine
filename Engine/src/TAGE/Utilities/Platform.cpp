#include "tagepch.h"
#include "Platform.h"

namespace TAGE {
    namespace Platform {
        uint64_t DateTime::GetCurrentDateTimeU64()
        {
            std::string string = GetCurrentDateTimeString();
            return std::stoull(string);
        }

        std::string DateTime::GetCurrentDateTimeString()
        {
            std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::tm* localTime = std::localtime(&currentTime);

            int year = localTime->tm_year + 1900;
            int month = localTime->tm_mon + 1;
            int day = localTime->tm_mday;
            int hour = localTime->tm_hour;
            int minute = localTime->tm_min;

            std::ostringstream oss;
            oss << year
                << std::setw(2) << std::setfill('0') << month
                << std::setw(2) << std::setfill('0') << day
                << std::setw(2) << std::setfill('0') << hour
                << std::setw(2) << std::setfill('0') << minute;

            return oss.str();
        }
    }
}