#include "stdafx.h"
#include "TestCommand.h"

#include "../Command/FireCommand.h"
#include "../Command/JumpCommand.h"
#include "../Command/MoveCommand.h"
#include "../Command/ChangeCommand.h"

TestCommand::TestCommand(ExecuteValues * values)
	:Execute(values)
{
	buttonX = buttonY = buttonA = buttonB = NULL;

	fire = new FireCommand();
	jump = new JumpCommand();
	move = new MoveCommand();
	change = new ChangeCommand();
}

TestCommand::~TestCommand()
{
	SAFE_DELETE(fire);
	SAFE_DELETE(jump);
	SAFE_DELETE(move);
	SAFE_DELETE(change);
}

void TestCommand::Update()
{

}

void TestCommand::PreRender()
{

}

void TestCommand::Render()
{
	// Command ImGui
	{
		ImGui::Begin("Command##1");

		ImGui::Selectable("Jump");
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("Command", &jump, sizeof(Command*));
			ImGui::Text("Drag Jump Command");
			ImGui::EndDragDropSource();
		}
		ImGui::Selectable("Fire");
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("Command", &fire, sizeof(Command*));
			ImGui::Text("Drag Fire Command");
			ImGui::EndDragDropSource();
		}
		ImGui::Selectable("Move");
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("Command", &move, sizeof(Command*));
			ImGui::Text("Drag Move Command");
			ImGui::EndDragDropSource();
		}
		ImGui::Selectable("Change");
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("Command", &change, sizeof(Command*));
			ImGui::Text("Drag Change Command");
			ImGui::EndDragDropSource();
		}

		ImGui::End();
	}


	static string btnXName = "None";
	static string btnYName = "None";
	static string btnAName = "None";
	static string btnBName = "None";

	// Button ImGui
	{
		ImGui::Begin("Button##1");

		// X Button
		ImGui::Selectable("X Button", false, 0, ImVec2(ImGui::GetWindowSize().x - 70, 0));
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Command")) {
				IM_ASSERT(payload->DataSize == sizeof(Command*));
				Command** command = (Command**)payload->Data;
				buttonX = *command;
				ImGui::Text("Drop X Button");

				buttonX->Execute();
				string text = ImGui::GetClipboardText();
				btnXName = text;
				logs.push_back("X Button's Command = " + text);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine(ImGui::GetWindowSize().x - 50);
		if (ImGui::SmallButton("Reset##X")) {
			buttonX = NULL;
			btnXName = "None";
			logs.push_back("X Button's Command Reset");
		}

		// Y Button
		ImGui::Selectable("Y Button", false, 0, ImVec2(ImGui::GetWindowSize().x - 70, 0));

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Command")) {
				IM_ASSERT(payload->DataSize == sizeof(Command*));
				Command** command = (Command**)payload->Data;
				buttonY = *command;
				ImGui::Text("Drop Y Button");

				buttonY->Execute();
				string text = ImGui::GetClipboardText();
				btnYName = text;
				logs.push_back("Y Button's Command = " + text);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine(ImGui::GetWindowSize().x - 50);
		if (ImGui::SmallButton("Reset##Y")) {
			buttonY = NULL;
			btnYName = "None";
			logs.push_back("Y Button's Command Reset");
		}

		// A Button
		ImGui::Selectable("A Button", false, 0, ImVec2(ImGui::GetWindowSize().x - 70, 0));

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Command")) {
				IM_ASSERT(payload->DataSize == sizeof(Command*));
				Command** command = (Command**)payload->Data;
				buttonA = *command;
				ImGui::Text("Drop A Button");

				buttonA->Execute();
				string text = ImGui::GetClipboardText();
				btnAName = text;
				logs.push_back("A Button's Command = " + text);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine(ImGui::GetWindowSize().x - 50);
		if (ImGui::SmallButton("Reset##A")) {
			buttonA = NULL;
			btnAName = "None";
			logs.push_back("A Button's Command Reset");
		}

		// B Button
		ImGui::Selectable("B Button", false, 0, ImVec2(ImGui::GetWindowSize().x - 70, 0));

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Command")) {
				IM_ASSERT(payload->DataSize == sizeof(Command*));
				Command** command = (Command**)payload->Data;
				buttonB = *command;
				ImGui::Text("Drop B Button");

				buttonB->Execute();
				string text = ImGui::GetClipboardText();
				btnBName = text;
				logs.push_back("B Button's Command = " + text);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine(ImGui::GetWindowSize().x - 50);
		if (ImGui::SmallButton("Reset##B")) {
			buttonB = NULL;
			btnBName = "None";
			logs.push_back("B Button's Command Reset");
		}

		ImGui::End();
	}

	static ImVec2 center;

	// Window ImGui
	{
		ImGui::Begin("Window##1");

		center = ImGui::GetWindowPos();
		float x = ImGui::GetWindowSize().x / 2;
		float y = ImGui::GetWindowSize().y / 2;
		float padding = 100;
		float padding2 = 10;
		float radius = 50;

		string itemName;
		ImVec2 itemPos;

		// x btn
		itemName = "X";
		itemPos = ImVec2(center.x + x - padding, center.y + y - padding);
		ImGui::GetWindowDrawList()->AddCircleFilled(itemPos, radius, IM_COL32(0, 0, 255, 255));
		itemPos.x -= padding2;
		itemPos.y -= padding2;
		ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), 30, itemPos, IM_COL32(0, 0, 0, 255), itemName.c_str());
		itemName = btnXName;
		itemPos.x -= padding2 * itemName.length() / 1.5f;
		itemPos.y += padding2 * 7;
		ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), 30, itemPos, IM_COL32(255, 255, 255, 255), itemName.c_str());

		// y btn
		itemName = "Y";
		itemPos = ImVec2(center.x + x + padding, center.y + y - padding);
		ImGui::GetWindowDrawList()->AddCircleFilled(itemPos, radius, IM_COL32(255, 255, 0, 255));
		itemPos.x -= padding2;
		itemPos.y -= padding2;
		ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), 30, itemPos, IM_COL32(0, 0, 0, 255), itemName.c_str());
		itemName = btnYName;
		itemPos.x -= padding2 * 2.5f;
		itemPos.y += padding2 * 7;
		ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), 30, itemPos, IM_COL32(255, 255, 255, 255), itemName.c_str());

		// a btn
		itemName = "A";
		itemPos = ImVec2(center.x + x - padding, center.y + y + padding);
		ImGui::GetWindowDrawList()->AddCircleFilled(itemPos, radius, IM_COL32(0, 255, 0, 255));
		itemPos.x -= padding2;
		itemPos.y -= padding2;
		ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), 30, itemPos, IM_COL32(0, 0, 0, 255), itemName.c_str());
		itemName = btnAName;
		itemPos.x -= padding2 * 2.5f;
		itemPos.y += padding2 * 7;
		ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), 30, itemPos, IM_COL32(255, 255, 255, 255), itemName.c_str());

		// b btn
		itemName = "B";
		itemPos = ImVec2(center.x + x + padding, center.y + y + padding);
		ImGui::GetWindowDrawList()->AddCircleFilled(itemPos, radius, IM_COL32(255, 0, 0, 255));
		itemPos.x -= padding2;
		itemPos.y -= padding2;
		ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), 30, itemPos, IM_COL32(0, 0, 0, 255), itemName.c_str());
		itemName = btnBName;
		itemPos.x -= padding2 * 2.5f;
		itemPos.y += padding2 * 7;
		ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), 30, itemPos, IM_COL32(255, 255, 255, 255), itemName.c_str());

		ImGui::End();
	}

	// Log
	{
		ImGui::Begin("Log##1");

		if (ImGui::Button("X Button##Excute")) {
			logs.push_back("X Button -> Execute() -> " + btnXName + "()");
		}
		ImGui::SameLine(150);
		if (ImGui::Button("Y Button##Excute")) {
			logs.push_back("Y Button -> Execute() -> " + btnYName + "()");
		}
		ImGui::SameLine(300);
		if (ImGui::Button("A Button##Excute")) {
			logs.push_back("A Button -> Execute() -> " + btnAName + "()");
		}
		ImGui::SameLine(450);
		if (ImGui::Button("B Button##Excute")) {
			logs.push_back("B Button -> Execute() -> " + btnBName + "()");
		}

		ImGui::TextColored(ImVec4(255, 255, 0, 255), "Log - Size : %d", logs.size());
		ImGui::Indent(); 
		for (int i = 0; i < logs.size(); i++) {
			ImGui::Text(("Log's %d : " + logs[i]).c_str(), i);
		}
		ImGui::Unindent();

		ImGui::End();
	}
}

void TestCommand::PostRender()
{

}



