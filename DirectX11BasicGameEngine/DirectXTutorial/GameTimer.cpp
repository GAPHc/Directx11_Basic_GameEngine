#include<Windows.h>
#include "GameTimer.h"

GameTimer::GameTimer()
	:m_SecondsPerCount(0.0),
	m_DeltaTime(-1.0),
	m_BaseTime(0),
	m_PauseTime(0),
	m_PrevTime(0),
	m_CurrTime(0),
	m_isStopped(false)
{
	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond); // 초당 개수 구함
	m_SecondsPerCount = 1.0 / (double)countsPerSecond; // 개수당 시간 구함
}

GameTimer::~GameTimer()
{
}

float GameTimer::TotalTime()const
{
	//If stopped, do not count the time
	//Moreover if already had a pause, between mStopTime, mBaseTime include paused time
	//time, which we do not want to count. So to correct this, subtract paused time from mStopTime:
	if (m_isStopped)
	{
		return (float)(((m_StopTime - m_PauseTime) - m_BaseTime)*m_SecondsPerCount);
	}

	//The distance mCurrentTime - mBasetTime includes pause time, which we do not want to count.
	//So we can subtract the pausetime from mCurrTime.
	else
	{
		return (float)(((m_CurrTime - m_PauseTime) - m_BaseTime)*m_SecondsPerCount);
	}
}

float GameTimer::DeltaTime()const
{
	return (float)m_DeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = 0;
	m_isStopped = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	//Accumulate the time elapsed between stop and start

	//If resumming timer frome stopped state,
	//Set pause, previous, stop time and change stop state 
	if (m_isStopped)
	{
		m_PauseTime += (startTime - m_StopTime);

		m_PrevTime = startTime;
		m_StopTime = 0;

		m_isStopped = false;
	}
}

void GameTimer::Stop()
{
	//If we are already stopped, don't execute this
	if (!m_isStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		//Save stopped time
		m_StopTime = currTime;
		//Set true stop checker
		m_isStopped = true;
	}
}

void GameTimer::Tick()
{
	if (m_isStopped)
	{
		m_DeltaTime = 0.0f;
		return;
	}
	//Get the time from this frame
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_CurrTime = currTime;

	//Get a difference between this frame and the previous frame
	m_DeltaTime = (m_CurrTime - m_PrevTime)*m_SecondsPerCount;

	//Prepare for next frame
	m_PrevTime = m_CurrTime;

	//Check DeltaTime that can be negative
	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0f;
	}
}