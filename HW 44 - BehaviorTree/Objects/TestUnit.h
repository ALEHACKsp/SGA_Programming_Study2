#pragma once

#include "Draw\IBlackboard.h"

static int Price[] = { 500, 800, 1000 };

class TestUnit : public IBlackboard
{
public:
	enum State { Idle, Move, Buy, Drink };
	enum Beverage { Water, Cola, Juice };

public:
	TestUnit();
	~TestUnit();

	//void Update();
	void Render();

#pragma region Blackboard

public:
	// IBlackboard을(를) 통해 상속됨
	virtual string BlackboardName() override;

	//virtual void BlackboardKeys(OUT vector<string>& keys, OUT map<string, function<void(OUT D3DXVECTOR3)>>& funcs, OUT map<string, string>& tips) override;

	virtual void BlackboardServices(OUT vector<string>& services, OUT map<string, function<void()>>& funcs, OUT map<string, string>& tips) override;

	virtual void BlackboardDecorators(OUT vector<string>& decorators, OUT map<string, function<bool()>>& funcs, OUT map<string, string>& tips) override;

	virtual void BlackboardTasks(OUT vector<string>& tasks, OUT map<string, function<TaskResult()>>& funcs, OUT map<string, string>& tips) override;

private:

	// Task
	TaskResult StandUp();
	TaskResult MoveTo();
	TaskResult BuyDrink();
	TaskResult SitDown();
	
	// Decorator
	bool IsMoneyEnough();
	//bool IsThirsty();

	// Service
	void Reset();

#pragma endregion

private:
	//float hp;
	int money;
	State state;

	Beverage choice;

	string target;
	int targetDist;

	stack<string> progress;
};
