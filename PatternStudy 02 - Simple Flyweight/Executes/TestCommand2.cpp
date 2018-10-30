#include "stdafx.h"
#include "TestCommand2.h"

#include "../Command/MoveUnitCommand.h"
#include "../Command/ShowUnitCommand.h"
#include "../Command/ColorUnitCommand.h"
#include "../Command/RadiusUnitCommand.h"

#include <stack>

TestCommand2::TestCommand2(ExecuteValues * values)
	:Execute(values)
	, selectUnit(-1)
{
	//units.push_back(new Unit(250, 250));
	//units.back()->SetName(to_string(units.size()));
	//units.push_back(new Unit(350, 350));
	//units.back()->SetName(to_string(units.size()));
	//selectUnit = 0;
}

TestCommand2::~TestCommand2()
{
	for (Unit * unit : units)
		SAFE_DELETE(unit);
	
	ClearUndoStack();
	ClearRedoStack();
}

void TestCommand2::Update()
{

}

void TestCommand2::PreRender()
{

}

void TestCommand2::Render()
{	
	static ImVec2 min, max;
	static float unitPos[] = { 250.0f, 250.0f };
	static float movePos[] = { 350.0f, 350.0f };

	// Unit ImGui
	{
		ImGui::Begin("Units##2");

		for (int i = 0; i < units.size(); i++) {
			ImGui::RadioButton(("Unit " + units[i]->GetName()).c_str(), &selectUnit, i);

			bool isShow = units[i]->GetIsShow();
			string showStr = "(Show)";
			
			ImGui::SameLine(80);
			if (isShow)
				ImGui::Text(showStr.c_str());
			else
				ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), showStr.c_str());

		}

		ImGui::End();
	}

	// Button ImGui
	{
		ImGui::Begin("Unit Info##2");

		float moveSpeed = 5.0f;

		if (selectUnit != -1 && selectUnit < units.size())
		{
			Unit* unit = units[selectUnit];
			ImVec2 pos = unit->Pos();
			ImGui::Text(("Unit's " + unit->GetName()).c_str());
			ImGui::Text("Position : %.2f %.2f", pos.x, pos.y);

			bool isShow = unit->GetIsShow();
			if (ImGui::Checkbox("IsShow", &isShow)) {
				unit->SetIsShow(isShow);
				undoStack.push(new ShowUnitCommand(unit, isShow));
				undoStack.top()->Execute();
			}

			//float padding = 70;
			//if (ImGui::Button("Up", ImVec2(50, 30))) {
			//	undoStack.push(new MoveUnitCommand(unit, pos.x, pos.y - moveSpeed));
			//	undoStack.top()->Execute();
			//}
			//ImGui::SameLine(padding);
			//if (ImGui::Button("Down", ImVec2(50, 30))) {
			//	undoStack.push(new MoveUnitCommand(unit, pos.x, pos.y + moveSpeed));
			//	undoStack.top()->Execute();
			//}
			//padding += 60;
			//ImGui::SameLine(padding);
			//if (ImGui::Button("Left", ImVec2(50, 30))) {
			//	undoStack.push(new MoveUnitCommand(unit, pos.x - moveSpeed, pos.y));
			//	undoStack.top()->Execute();
			//}
			//padding += 60;
			//ImGui::SameLine(padding);
			//if (ImGui::Button("Right", ImVec2(50, 30))) {
			//	undoStack.push(new MoveUnitCommand(unit, pos.x + moveSpeed, pos.y));
			//	undoStack.top()->Execute();
			//}

			if (ImGui::Button("Move")) {
				undoStack.push(new MoveUnitCommand(unit, movePos[0], movePos[1]));
				undoStack.top()->Execute();
				movePos[0] = Math::Random(min.x, max.x);
				movePos[1] = Math::Random(min.y, max.y);
			}
			ImGui::SameLine(80);
			ImGui::DragFloat2("##movePos", movePos, 0.1f);

			float radius = unit->GetRadius();
			float range = 2.5f;
			if (ImGui::DragFloat("Radius", &radius, 0.1f)) {
				if (fabs(unit->GetRadius() - radius) > range) {
					undoStack.push(new RadiusUnitCommand(unit, radius));
					undoStack.top()->Execute();
				}
			}

			D3DXCOLOR color = unit->GetColor();
			if (ImGui::ColorEdit4("Color", (float*)color)) {
				
				if (color != unit->GetColor()) {
					undoStack.push(new ColorUnitCommand(unit, color));
					undoStack.top()->Execute();
				}
			}
		}

		ImGui::End();
	}


	// test
	//ImGui::Text("%.2f %.2f", ImGui::GetMousePos().x, ImGui::GetMousePos().y);

	ImGui::StyleColorsClassic();
	// Window ImGui
	{
		ImGui::Begin("Window##2");

		for (Unit* unit : units) {
			D3DXCOLOR color = unit->GetColor();

			if (unit->GetIsShow()) {
				ImGui::GetWindowDrawList()->AddCircleFilled(
					unit->Pos(), unit->GetRadius(),
					IM_COL32(color.r * 255, color.g * 255, color.b * 255, color.a * 255));
			}
		}

		min = ImGui::GetWindowPos();
		max = ImVec2(min.x + ImGui::GetWindowSize().x, min.y + ImGui::GetWindowSize().y);
		//max = ImGui::GetWindowSize();

		ImGui::End();
	}
	ImGui::StyleColorsDark();


	// Create Unit
	{
		ImGui::Begin("Create Unit");

		if (ImGui::Button("Create")) {
			units.push_back(new Unit(unitPos[0], unitPos[1]));
			units.back()->SetName(to_string(units.size()));

			unitPos[0] = Math::Random(min.x, max.x);
			unitPos[1] = Math::Random(min.y, max.y);
		}

		ImGui::SameLine(80);
		ImGui::DragFloat2("##unitPos", unitPos, 0.1f);

		ImGui::End();
	}

	// Undo Stack
	{
		ImGui::Begin("Undo Stack##2");

		if (ImGui::Button("Undo")) Undo();
		ImGui::SameLine(50);
		if (ImGui::Button("Reset")) ClearUndoStack();

		stack<Command*> temp = undoStack;
		stack<Command*> temp2;

		while (!temp.empty()) {
			temp2.push(temp.top());
			temp.pop();
		}

		while (!temp2.empty()) {
			temp2.top()->Render();
			temp2.pop();
		}

		ImGui::End();
	}

	// Redo Stack
	{
		ImGui::Begin("Redo Stack##2");

		if (ImGui::Button("Redo")) Redo();
		ImGui::SameLine(50);
		if (ImGui::Button("Reset")) ClearRedoStack();

		stack<Command*> temp = redoStack;
		stack<Command*> temp2;

		while (!temp.empty()) {
			temp2.push(temp.top());
			temp.pop();
		}

		while (!temp2.empty()) {
			temp2.top()->Render();
			temp2.pop();
		}

		ImGui::End();
	}

}

void TestCommand2::PostRender()
{

}

void TestCommand2::Undo()
{
	if (!undoStack.empty()) {
		Command* command = undoStack.top();
		command->Undo();
		redoStack.push(command);
		undoStack.pop();
	}
}

void TestCommand2::Redo()
{
	if (!redoStack.empty()) {
		Command* command = redoStack.top();
		command->Execute();
		undoStack.push(command);
		redoStack.pop();
	}
}

void TestCommand2::ClearUndoStack()
{
	while (!undoStack.empty()) {
		Command * command = undoStack.top();
		SAFE_DELETE(command);
		undoStack.pop();
	}
}

void TestCommand2::ClearRedoStack()
{
	while (!redoStack.empty()) {
		SAFE_DELETE(redoStack.top());
		redoStack.pop();
	}
}



