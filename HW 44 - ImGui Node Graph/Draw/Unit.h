#pragma once

// Behavior Tree �׽�Ʈ��
/*
	Unit���� �ϴ� ����� ���� GameModel Mesh ��� ����ϸ� �ȴ�.
	IBlackboard ��ӹ޾� BehaviorTree���� ����� �Լ����� Key�� �Բ� �Լ������ͷ� ������ �Ѱ��ֵ��� �� ��
	BehaviorTree���� �Լ������ͷ� �ش� Task�� ������ �� �ֵ��� �Ѵ�.

	������ Task���� return ������ true or false�� ��ȯ�ϵ��� �����ϵ����Ѵ�. (Behavior Tree ���� Ư�� ����)
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

	// IBlackboard��(��) ���� ��ӵ�
	virtual string BlackboardName() override;

	//virtual void BlackboardKeys(OUT vector<string>& keys, OUT map<string, function<void(OUT D3DXVECTOR3)>>& funcs) override;
	virtual void BlackboardServices(OUT vector<string>& services, OUT map<string, function<void()>>& funcs) override;
	virtual void BlackboardDecorators(OUT vector<string>& decorators, OUT map<string, function<bool()>>& funcs) override;
	virtual void BlackboardTasks(OUT vector<string>& tasks, OUT map<string, function<TaskResult()>>& funcs) override;

private:
	string name;
	D3DXVECTOR3 position;
	State state;
};