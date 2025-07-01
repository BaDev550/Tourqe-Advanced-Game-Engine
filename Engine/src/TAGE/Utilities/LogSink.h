#pragma once
#include "spdlog/sinks/base_sink.h"
#include <mutex>
#include <vector>
#include <string>

namespace TAGE {
	class LogSink : public spdlog::sinks::base_sink<std::mutex> {
	public:
		struct LogMessage {
			spdlog::level::level_enum level;
			std::string message;
		};

		const std::vector<LogMessage>& GetMessages() const { return _messages; }
		void Clear() { _messages.clear(); }

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override {
			spdlog::memory_buf_t formatted;
			formatter_->format(msg, formatted);

			_messages.push_back({ msg.level, fmt::to_string(formatted) });
		}

		void flush_() override {}

	private:
		std::vector<LogMessage> _messages;
	};
}