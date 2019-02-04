#pragma once

#include "stdafx.h"

enum TaskResult { Result_False, Result_True, Result_Running};

class IBlackboard
{
public:
	IBlackboard() {}
	virtual ~IBlackboard() {}

	// �����忡�� ������ �̸� �������� �������� ������ �� ������ ���� ���
	virtual string BlackboardName() = 0;

	// �ϴ� �̰� ���� Key�� �ϴ°� �ָ��� �ϴ� Task�� �� ó��
	//virtual void BlackboardKeys(OUT vector<string>& keys, OUT map<string, function<void(OUT D3DXVECTOR3)> >& funcs) = 0;
	virtual void BlackboardServices(OUT vector<string>& services, OUT map<string, function<void()> >& funcs) = 0;
	virtual void BlackboardDecorators(OUT vector<string>& decorators, OUT map<string, function<bool()> >& funcs) = 0;
	virtual void BlackboardTasks(OUT vector<string>& tasks, OUT map<string, function<TaskResult()> >& funcs) = 0;
};