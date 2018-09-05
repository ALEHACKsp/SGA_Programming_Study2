#include "stdafx.h"
#include "Memory.h"

#include "Objects\GameRender.h"

stack<MemoryData> Memory::undoStack;
stack<MemoryData> Memory::redoStack;

Memory::Memory()
{
	
}

Memory::~Memory()
{
}

void Memory::Do(GameRender * target, Memory_Kind kind, D3DXVECTOR3 val)
{
	MemoryData data;
	data.target = target;
	data.kind = kind;
	data.val = val;

	undoStack.push(data);
}
bool Memory::UnDo()
{
	if (undoStack.empty())
		return false;

	MemoryData data = undoStack.top();
	data.target->SetIsMemory(false);
	D3DXVECTOR3 temp;
	switch (data.kind)
	{
	case Memory_Create:
		data.target->SetIsLive(false);
		break;
	case Memory_Delete:
		data.target->SetIsLive(true);
		break;
	case Memory_Position:
		temp = data.val;
		data.val = data.target->Position();
		data.target->Position(temp);
		break;
	case Memory_Rotation:
		temp = data.val;
		data.val = data.target->Rotation();
		data.target->Rotation(temp);
		break;
	case Memory_Scale:
		temp = data.val;
		data.val = data.target->Scale();
		data.target->Scale(temp);
		break;
	}

	redoStack.push(data);
	undoStack.pop();

	return true;
}

bool Memory::ReDo()
{
	if (redoStack.empty())
		return false;

	MemoryData data = redoStack.top();
	data.target->SetIsMemory(false);
	D3DXVECTOR3 temp;
	switch (data.kind)
	{
	case Memory_Create:
		data.target->SetIsLive(true);
		break;
	case Memory_Delete:
		data.target->SetIsLive(false);
		break;
	case Memory_Position:
		temp = data.val;
		data.val = data.target->Position();
		data.target->Position(temp);
		break;
	case Memory_Rotation:
		temp = data.val;
		data.val = data.target->Rotation();
		data.target->Rotation(temp);
		break;
	case Memory_Scale:
		temp = data.val;
		data.val = data.target->Scale();
		data.target->Scale(temp);
		break;
	}

	undoStack.push(data);
	redoStack.pop();

	return true;
}

void Memory::PostRender()
{
	ImGui::Begin("Memory");

	ImGui::TextColored(ImVec4(1,1,0,1), "undo Stack");

	stack<MemoryData> temp = undoStack;
	while (!temp.empty())
	{
		MemoryData data = temp.top();
		ImGui::Text(data.target->GetName().c_str());
		ImGui::SameLine(60);
		switch (data.kind)
		{
		case Memory_Create:
			ImGui::Text("Kind : Create");
			break;
		case Memory_Delete:
			ImGui::Text("Kind : Delete");
			break;
		case Memory_Position:
			ImGui::Text("Kind : Position (%.2f %.2f %.2f)",
				data.val.x, data.val.y, data.val.z);
			break;
		case Memory_Rotation:
			ImGui::Text("Kind : Rotation (%.2f %.2f %.2f)",
				data.val.x, data.val.y, data.val.z);
			break;
		case Memory_Scale:
			ImGui::Text("Kind : Scale (%.2f %.2f %.2f)",
				data.val.x, data.val.y, data.val.z);
			break;
		}
		ImGui::Separator();
		temp.pop();
	}

	ImGui::Separator();
	
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Redo Stack");

	temp = redoStack;
	while (!temp.empty())
	{
		MemoryData data = temp.top();
		ImGui::Text(data.target->GetName().c_str());
		ImGui::SameLine(60);
		switch (data.kind)
		{
		case Memory_Create:
			ImGui::Text("Kind : Create");
			break;
		case Memory_Delete:
			ImGui::Text("Kind : Delete");
			break;
		case Memory_Position:
			ImGui::Text("Kind : Position (%.2f %.2f %.2f)",
				data.val.x, data.val.y, data.val.z);
			break;
		case Memory_Rotation:
			ImGui::Text("Kind : Rotation (%.2f %.2f %.2f)",
				data.val.x, data.val.y, data.val.z);
			break;
		case Memory_Scale:
			ImGui::Text("Kind : Scale (%.2f %.2f %.2f)",
				data.val.x, data.val.y, data.val.z);
			break;
		}
		temp.pop();
	}

	ImGui::End();
}
