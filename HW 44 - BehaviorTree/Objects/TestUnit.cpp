#include "stdafx.h"
#include "TestUnit.h"

TestUnit::TestUnit()
	: state(State::Idle), money(10000)
{
}

TestUnit::~TestUnit()
{
}

//void TestUnit::Update()
//{
//}

void TestUnit::Render()
{
	ImGui::Begin("Unit");

	ImGui::Text("Money : %d", money);

	string str = "";
	switch (state)
	{
	case TestUnit::Idle: str = "Idle";
		break;
	case TestUnit::Move: str = "Move";
		break;
	case TestUnit::Buy: str = "Buy";
		break;
	case TestUnit::Drink: str = "Drink";
		break;
	}

	ImGui::Columns(2);
	ImGui::Text("State");
	ImGui::NextColumn();
	ImGui::Text(str.c_str());
	ImGui::Columns();
	ImGui::Separator();
	ImGui::LabelText("##Progress", "Progress");
	stack<string> temp = progress;

	while (temp.empty() == false) {
		ImGui::Text(temp.top().c_str());
		temp.pop();
	}

	ImGui::End();
}

string TestUnit::BlackboardName()
{
	return "Unit";
}

//void TestUnit::BlackboardKeys(OUT vector<string>& keys, OUT map<string, function<void(OUT D3DXVECTOR3)>>& funcs, OUT map<string, string>& tips)
//{
//}

void TestUnit::BlackboardServices(OUT vector<string>& services, OUT map<string, function<void()>>& funcs, OUT map<string, string>& tips)
{
	services.push_back("Reset");
	funcs.insert(make_pair(services.back(), bind(&TestUnit::Reset, this)));
	tips[services.back()] = "Unit's Progress Stack Reset (Service)";
}

void TestUnit::BlackboardDecorators(OUT vector<string>& decorators, OUT map<string, function<bool()>>& funcs, OUT map<string, string>& tips)
{
	decorators.push_back("IsMoneyEnough");
	funcs.insert(make_pair(decorators.back(), bind(&TestUnit::IsMoneyEnough, this)));
	tips[decorators.back()] = "Unit Check Money To Buy Drink (Decorator)";
}

void TestUnit::BlackboardTasks(OUT vector<string>& tasks, OUT map<string, function<TaskResult()>>& funcs, OUT map<string, string>& tips)
{
	tasks.push_back("StandUp");
	funcs.insert(make_pair(tasks.back(), bind(&TestUnit::StandUp, this)));
	tips[tasks.back()] = "Unit Stand Up (Task)";

	tasks.push_back("MoveToVendingMachine");
	funcs.insert(make_pair(tasks.back(), bind(&TestUnit::MoveTo, this)));
	tips[tasks.back()] = "Unit Move To VendingMachine (maybe Animation) (Task)";

	tasks.push_back("BuyDrink");
	funcs.insert(make_pair(tasks.back(), bind(&TestUnit::BuyDrink, this)));
	tips[tasks.back()] = "Unit Buy Drink (Task)";

	tasks.push_back("MoveToMyPosition");
	funcs.insert(make_pair(tasks.back(), bind(&TestUnit::MoveTo, this)));
	tips[tasks.back()] = "Unit Move To My Position (maybe Animation) (Task)";

	tasks.push_back("SitDown");
	funcs.insert(make_pair(tasks.back(), bind(&TestUnit::SitDown, this)));
	tips[tasks.back()] = "Unit Sit Down (Task)";
}

TaskResult TestUnit::StandUp()
{
	if (state != State::Idle) 
		state = State::Idle;
	
	progress.push("Unit : Stand Up");

	return TaskResult::Result_True;
}

TaskResult TestUnit::MoveTo()
{
	// 처음 호출
	if (state != State::Move) {
		state = State::Move;
		progress.push("Unit : Move Start");
		
		return TaskResult::Result_Running;
	}
	else if (targetDist >= 0) {
		string temp = "Dist : " + to_string(targetDist--);
		progress.push(temp);

		progress.push("Unit : Move To " + target);

		// 0보다 크면 아직 진행 0이면 도착
		if (targetDist >= 0)
			return TaskResult::Result_Running;
		else
			return TaskResult::Result_True;
	}

	return TaskResult::Result_True;
}

TaskResult TestUnit::BuyDrink()
{
	if (state != State::Buy) {
		state = State::Buy;

		string temp = "Result : " + to_string(money) + " - " + to_string(Price[choice]) + " = ";
		money -= Price[choice];
		temp += to_string(money);

		progress.push("Unit : Buy Drink");

		target = "My Position";
		targetDist = 5;

		progress.push(temp);
	}

	return TaskResult::Result_True;
}

TaskResult TestUnit::SitDown()
{
	if (state != State::Idle) 
		state = State::Idle;

	progress.push("Unit : Sit Down");

	return TaskResult::Result_True;
}

bool TestUnit::IsMoneyEnough()
{

	choice = (Beverage)Math::Random(Beverage::Water, Beverage::Juice);

	string temp = "Choice : ";

	switch (choice)
	{
	case TestUnit::Water: temp += "Water";
		break;
	case TestUnit::Cola: temp += "Cola";
		break;
	case TestUnit::Juice: temp += "Juice";
		break;
	}

	temp += "(" + to_string(Price[choice]) + ")";

	progress.push(temp);

	if (money < Price[choice]) {
		progress.push("Result : Lack of Money");
		return TaskResult::Result_False;
	}
	else {
		progress.push("Result : Enough Money");


		target = "VendingMachine";
		targetDist = 5;

		return TaskResult::Result_True;
	}
	
	return TaskResult::Result_True;
}

void TestUnit::Reset()
{
	while (!progress.empty())
		progress.pop();
}
