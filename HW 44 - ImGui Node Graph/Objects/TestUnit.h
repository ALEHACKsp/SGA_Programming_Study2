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

	//virtual void BlackboardKeys(OUT vector<string>& keys, OUT map<string, function<void(OUT D3DXVECTOR3)>>& funcs) override;

	virtual void BlackboardServices(OUT vector<string>& services, OUT map<string, function<void()>>& funcs) override;

	virtual void BlackboardDecorators(OUT vector<string>& decorators, OUT map<string, function<bool()>>& funcs) override;

	virtual void BlackboardTasks(OUT vector<string>& tasks, OUT map<string, function<TaskResult()>>& funcs) override;

private:

	// Task
	TaskResult StandUp();
	TaskResult MoveTo();
	TaskResult BuyDrink();
	TaskResult SitDown();
	
	// Decorator
	bool IsMoneyEnough();
	//bool IsThirsty();

#pragma endregion

private:
	//float hp;
	int money;
	State state;

	bool bChoice;
	Beverage choice;

	string target;
	int targetDist;

	bool bBuyDrink;

	stack<string> progress;
};
