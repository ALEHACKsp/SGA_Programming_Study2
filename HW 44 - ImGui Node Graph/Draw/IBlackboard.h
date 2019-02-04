#pragma once

#include "stdafx.h"

enum TaskResult { Result_False, Result_True, Result_Running};

class IBlackboard
{
public:
	IBlackboard() {}
	virtual ~IBlackboard() {}

	// 블랙보드에서 쓰여질 이름 블랙보드의 여러개가 쓰여질 때 구분을 위해 사용
	virtual string BlackboardName() = 0;

	// 일단 이거 제외 Key로 하는건 애매함 일단 Task로 다 처리
	//virtual void BlackboardKeys(OUT vector<string>& keys, OUT map<string, function<void(OUT D3DXVECTOR3)> >& funcs) = 0;
	virtual void BlackboardServices(OUT vector<string>& services, OUT map<string, function<void()> >& funcs) = 0;
	virtual void BlackboardDecorators(OUT vector<string>& decorators, OUT map<string, function<bool()> >& funcs) = 0;
	virtual void BlackboardTasks(OUT vector<string>& tasks, OUT map<string, function<TaskResult()> >& funcs) = 0;
};