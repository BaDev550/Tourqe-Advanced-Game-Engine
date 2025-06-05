#pragma once

namespace TAGE {
	class TimeStep {
	public:
		TimeStep(float time = 0.0f) : _Time(time) {}

		operator float() const { return _Time; }

		float GetSeconds() const { return _Time; }
		float GetMilliseconds() const { return _Time * 1000.0f; }
		float GetFPS() const { return (_Time > 0) ? 1.0f / _Time : 0.0f; }
	private:
		float _Time;
	};
}
