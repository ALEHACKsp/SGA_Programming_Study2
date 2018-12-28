#pragma once

class Timer
{
public:
	Timer();
	~Timer();

	bool CoolTime(float setTime);
	void SetTime(float setTime, bool reset = false);
	bool CkeckTimeOver(bool loop = false);
	float SpendTime();
	float RemainTime();

private:
	float startTime;
	float setTime;
};