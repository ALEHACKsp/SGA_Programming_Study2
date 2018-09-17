#include "stdafx.h"
#include "TestImGuiDragnDrop.h"

#include "../Objects/GameAnimModel.h"

TestImGuiDragnDrop::TestImGuiDragnDrop(ExecuteValues * values)
	:Execute(values)
{

}

TestImGuiDragnDrop::~TestImGuiDragnDrop()
{

}

void TestImGuiDragnDrop::Update()
{

}

void TestImGuiDragnDrop::PreRender()
{

}

void TestImGuiDragnDrop::Render()
{

}

void TestImGuiDragnDrop::PostRender()
{
	static int a = 1;
	static int b = 2;
	static int c = 3;
	static int d = 4;


	ImGui::Begin("Window##1");

	//ImGui::Text("Item a %d", a);
	ImGui::Selectable(("Item a " + to_string(a)).c_str());
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("item#a", &a, sizeof(int));
		ImGui::Text("Drag Item a : %d", a);
		ImGui::EndDragDropSource();
	}

	ImGui::Selectable(("Item c " + to_string(c)).c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("item#b"))
		{
			IM_ASSERT(payload->DataSize == sizeof(int));
			int* data = (int*)payload->Data;
			c = *data;
			ImGui::Text("Drop Item");
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();

	ImGui::Begin("Window##2");

	//ImGui::Text("Item b %d", b);
	ImGui::Selectable(("Item b " + to_string(b)).c_str());
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("item#b", &b, sizeof(int));
		ImGui::Text("Drag Item b : %d", b);
		ImGui::EndDragDropSource();
	}

	ImGui::Selectable(("Item d " + to_string(d)).c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("item#a"))
		{
			IM_ASSERT(payload->DataSize == sizeof(int));
			int* data = (int*)payload->Data;
			d = *data;
			ImGui::Text("Drop Item");
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();
}

