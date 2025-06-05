#pragma once  
#include <chrono>  
#include <thread>  
#include <functional>

namespace TAGE {
class Timer {  
public:  
	Timer();  
	void Reset();  
	void Update(float deltaTime);
	float Elapsed() const;  
	float ElapsedMillis() const;  
	bool HasElapsed(float seconds) const {  
		return Elapsed() >= seconds;  
	}  
	static void Delay(float seconds) {  
		std::this_thread::sleep_for(std::chrono::duration<float>(seconds));  
	}  
	static void SetTimeout(float delaySeconds, std::function<void()> callback);
private:  
	std::chrono::time_point<std::chrono::high_resolution_clock> _Start;  
};  
}