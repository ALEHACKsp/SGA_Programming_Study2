#include "stdafx.h"
#include "Timer.h"

Timer::Timer()
	: setTime(0), startTime(0)
{
}

Timer::~Timer()
{
}

bool Timer::CoolTime(float setTime)
{
	//Ÿ�̸Ӱ� �������� �ʾҴٸ� Ÿ�̸� ����
	if (this->setTime == 0)
	{
		this->setTime = setTime;
		startTime = Time::Get()->Running();
		return true;
	}

	//Ÿ�̸� ���� ���� �˻�
	if (Time::Get()->Running() - startTime >= this->setTime)
	{
		this->setTime = 0;
		return true;
	}

	return false;
}

void Timer::SetTime(float setTime, bool reset)
{
	if (reset == false)
	{
		if (this->setTime >= Time::Get()->Running() - startTime) return;
	}

	this->setTime = setTime;
	startTime = Time::Get()->Running();
}

bool Timer::CkeckTimeOver(bool loop)
{
	if (startTime == 0) return true;
	if (Time::Get()->Running() - startTime >= setTime)
	{
		if(loop == true) startTime = Time::Get()->Running();
		else startTime = 0;
		return true;
	}
	return false;
}

float Timer::SpendTime()
{
	return Time::Get()->Running() - startTime;
}

float Timer::RemainTime()
{
	float remainTime = setTime - Time::Get()->Running() - startTime;
	return (remainTime > 0) ? remainTime : 0;
}
