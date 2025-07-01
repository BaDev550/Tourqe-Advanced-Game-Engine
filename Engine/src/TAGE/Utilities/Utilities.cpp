#include "tagepch.h"
#include "Logger.h"
#include "Timer.h"

namespace TAGE {
	MEM::Ref<spdlog::logger> Logger::s_CoreLogger;
	MEM::Ref<spdlog::logger> Logger::s_ClientLogger;

	struct TimerTask {
		float timeRemaining;
		std::function<void()> callback;
	};
	static std::vector<TimerTask> s_Timers;

	Timer::Timer() {
		Reset();
	}
	void Timer::Reset() {
		_Start = std::chrono::high_resolution_clock::now();
		s_Timers.clear();
	}
	void Timer::Update(float deltaTime)
	{
		for (size_t i = 0; i < s_Timers.size();) {
			auto& timer = s_Timers[i];
			timer.timeRemaining -= deltaTime;
			if (timer.timeRemaining <= 0.0f) {
				timer.callback();
				s_Timers.erase(s_Timers.begin() + i);
			}
			else {
				++i;
			}
		}
	}
	float Timer::Elapsed() const {
		return std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - _Start).count();
	}
	float Timer::ElapsedMillis() const {
		return Elapsed() * 1000.0f;
	}
	void Timer::SetTimeout(float delaySeconds, std::function<void()> callback)
	{
		s_Timers.push_back({ delaySeconds, std::move(callback) });
	}
}