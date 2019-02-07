#pragma once

// Behavior Tree 테스트용
/*
	Unit에서 하는 방법과 같이 GameModel Mesh 등에서 사용하면 된다.
	IBlackboard 상속받아 BehaviorTree에서 사용할 함수들을 Key와 함께 함수포인터로 구현해 넘겨주도록 한 뒤
	BehaviorTree에서 함수포인터로 해당 Task를 실행할 수 있도록 한다.

	구현할 Task들은 return 값으로 true or false를 반환하도록 구현하도록한다. (Behavior Tree 동작 특성 때문)
*/

#include "IBlackboard.h"

class Unit : public IBlackboard
{
public:
	enum State { IDLE, SEARCH, MOVE, ATTACK, DIE };

public:
	Unit(string name = "Unit");
	~Unit();

	void Render();

	TaskResult Idle();
	TaskResult Search();
	TaskResult Move();
	TaskResult Attack();
	TaskResult Die();

	void ServiceTest() {}
	bool DecoratorTest() { return true; }

	// IBlackboard을(를) 통해 상속됨
	virtual string BlackboardName() override;

	//virtual void BlackboardKeys(OUT vector<string>& keys, OUT map<string, function<void(OUT D3DXVECTOR3)>>& funcs, OUT map<string, string>& tips) override;
	virtual void BlackboardServices(OUT vector<string>& services, OUT map<string, function<void()>>& funcs, OUT map<string, string>& tips) override;
	virtual void BlackboardDecorators(OUT vector<string>& decorators, OUT map<string, function<bool()>>& funcs, OUT map<string, string>& tips) override;
	virtual void BlackboardTasks(OUT vector<string>& tasks, OUT map<string, function<TaskResult()>>& funcs, OUT map<string, string>& tips) override;

private:
	string name;
	D3DXVECTOR3 position;
	State state;
};