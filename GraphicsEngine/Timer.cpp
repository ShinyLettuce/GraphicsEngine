#include "Timer.h"

void Timer::SetIsPaused(bool aPauseState)
{
	myIsPaused = aPauseState;
	if (!myIsPaused)
	{
		myWasJustResumed = true;
	}
}

Timer::Timer(float aFixedTickTime) : myFixedTickTime(aFixedTickTime)
{
	myTimeAtCreation = static_cast<double>(myClock.now().time_since_epoch().count());
	myCurrentFixedRateTickCount = GetFixedRateTickCount();
	myLastFixedRateTickCount = myCurrentFixedRateTickCount;
}
   
void Timer::Update()
{
	if (myIsPaused)
	{
		return;
	}
	if (myWasJustResumed)
	{
		myCurrentFrameTime = (myClock.now().time_since_epoch().count() - myTimeAtCreation) / 1000000000.0;
		myLastFrameTime = myCurrentFrameTime - myFixedTickTime;
		myWasJustResumed = false;
	}
	else
	{
		myLastFrameTime = myCurrentFrameTime;
		myCurrentFrameTime = (myClock.now().time_since_epoch().count() - myTimeAtCreation) / 1000000000.0;
	}
	myLastFixedRateTickCount = myCurrentFixedRateTickCount;
	myCurrentFixedRateTickCount = GetFixedRateTickCount();
}

float Timer::GetDeltaTime() const
{
	return static_cast<float>(myCurrentFrameTime - myLastFrameTime);
}

double Timer::GetTotalTime() const
{
	return myCurrentFrameTime;
}

int Timer::GetFixedRateTickDeltaCount() const
{
	return static_cast<int>(myCurrentFixedRateTickCount - myLastFixedRateTickCount);
}

uint64_t Timer::GetFixedRateTickCount() const
{
	return static_cast<uint64_t>(myCurrentFrameTime / myFixedTickTime);
}
