#pragma once
class GameTimer
{
	private:
		double m_SecondsPerCount;
		double m_DeltaTime; //Delta time

		__int64 m_BaseTime; //Current time
		__int64 m_PauseTime;  //Temporary stop time
		__int64 m_StopTime; //Totally stop time
		__int64 m_PrevTime; //Previous time
		__int64 m_CurrTime; //Current time

	public:
		GameTimer();//Constructor
		virtual ~GameTimer();

		float TotalTime()const; //In seconds
		float DeltaTime()const; //In seconds

		void Reset(); //Call before message loop
		void Start(); //Call when start
		void Stop(); //Call when pause
		void Tick(); //Call every frame

		bool m_isStopped; 
};