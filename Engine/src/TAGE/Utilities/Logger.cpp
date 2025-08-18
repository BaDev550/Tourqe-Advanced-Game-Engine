#include "tagepch.h"
#include "Platform.h"
#include "TAGE/Project/Project.h"

namespace TAGE {
	MEM::Ref<spdlog::logger> Logger::s_CoreLogger;
	MEM::Ref<spdlog::logger> Logger::s_ClientLogger;

    void Logger::createFileLogger()
    {
        s_FileSink = MEM::MakeRef<spdlog::sinks::basic_file_sink_mt>((Project::GetActive()->GetCacheDirectory() / "Logs" / "Current-Log.txt").string(), true);
        s_CoreLogger->sinks().push_back(s_FileSink);
        s_ClientLogger->sinks().push_back(s_FileSink);
    }

    void Logger::SaveLogToCache()
	{
        const std::filesystem::path logDir = Project::GetActive()->GetCacheDirectory() / "Logs";
        std::filesystem::create_directories(logDir);
        auto newPath = logDir / (Platform::DateTime::GetCurrentDateTimeString() + "-log.txt");

        s_FileSink->flush();
        s_CoreLogger->sinks().erase(std::remove(s_CoreLogger->sinks().begin(), s_CoreLogger->sinks().end(), s_FileSink), s_CoreLogger->sinks().end());
        s_ClientLogger->sinks().erase(std::remove(s_ClientLogger->sinks().begin(), s_ClientLogger->sinks().end(), s_FileSink), s_ClientLogger->sinks().end());
        s_FileSink.reset();

        std::filesystem::rename(logDir / "Current-Log.txt", newPath);
	}
}