#include "stdafx.h"
#include "Unit.h"

Unit::Unit(string name)
	:state(Unit::IDLE), name(name)
{
}

Unit::~Unit()
{
}

void Unit::Render()
{
	ImGui::Begin("Unit");

	ImGui::Columns(2);

	ImGui::Text("State");

	ImGui::NextColumn();

	switch (state)
	{
	case Unit::IDLE: ImGui::Text("Idle"); break;
	case Unit::SEARCH: ImGui::Text("Search"); break;
	case Unit::MOVE: ImGui::Text("Move"); break;
	case Unit::ATTACK: ImGui::Text("Attack"); break;
	case Unit::DIE: ImGui::Text("Die"); break;
	}

	ImGui::Columns();

	ImGui::End();
}

TaskResult Unit::Idle()
{
	state = State::IDLE;
	return TaskResult::Result_True;
}

TaskResult Unit::Search()
{
	state = State::SEARCH;
	return TaskResult::Result_True;
}

TaskResult Unit::Move()
{
	state = State::MOVE;
	return TaskResult::Result_True;
}

TaskResult Unit::Attack()
{
	state = State::ATTACK;
	return TaskResult::Result_True;
}

TaskResult Unit::Die()
{
	state = State::DIE;
	return TaskResult::Result_True;
}

string Unit::BlackboardName()
{
	return name;
}

//void Unit::BlackboardKeys(OUT vector<string>& keys, OUT map<string, function<void(OUT D3DXVECTOR3)>>& funcs, OUT map<string, string>& tips)
//{
//}

void Unit::BlackboardServices(OUT vector<string>& services, OUT map<string, function<void()>>& funcs, OUT map<string, string>& tips)
{
	services.push_back("Test");
	funcs.insert(make_pair(services.back(), bind(&Unit::ServiceTest, this)));
}

void Unit::BlackboardDecorators(OUT vector<string>& decorators, OUT map<string, function<bool()>>& funcs, OUT map<string, string>& tips)
{
	decorators.push_back("Test");
	funcs.insert(make_pair(decorators.back(), bind(&Unit::DecoratorTest, this)));
}

void Unit::BlackboardTasks(OUT vector<string>& tasks, OUT map<string, function<TaskResult()>>& funcs, OUT map<string, string>& tips)
{
	tasks.push_back("Idle");
	funcs.insert(make_pair(tasks.back(), bind(&Unit::Idle, this)));
	tasks.push_back("Search");
	funcs.insert(make_pair(tasks.back(), bind(&Unit::Search, this)));
	tasks.push_back("Move");
	funcs.insert(make_pair(tasks.back(), bind(&Unit::Move, this)));
	tasks.push_back("Attack");
	funcs.insert(make_pair(tasks.back(), bind(&Unit::Attack, this)));
	tasks.push_back("Die");
	funcs.insert(make_pair(tasks.back(), bind(&Unit::Die, this)));
}
