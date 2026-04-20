#pragma once
#include <cstdint>
#include <chrono>

class Timer
{
public:
	Timer(float aFixedTickTime = 1.f / 60.f);
	Timer(const Timer& aTimer) = delete;
	Timer& operator=(const Timer& aTimer) = delete;

	void Update();

	float GetDeltaTime() const;
	double GetTotalTime() const;

	int GetFixedRateTickDeltaCount() const;
	uint64_t GetFixedRateTickCount() const;

	void SetIsPaused(bool aPauseState);

private:
	double myTimeAtCreation = 0;
	double myCurrentFrameTime = 0;
	double myLastFrameTime = 0;

	uint64_t myCurrentFixedRateTickCount = 0;
	uint64_t myLastFixedRateTickCount = 0;

	float myFixedTickTime = 0;

	bool myWasJustResumed = false;
	bool myIsPaused = false;
	std::chrono::high_resolution_clock myClock;
};
