#include "stdafx.h"
#include "DebugDrawLine.h"

DebugDrawLine::DebugDrawLine(D3DXVECTOR3 & start, D3DXVECTOR3 & end)
{
	name = "Line";

	this->start = start;
	this->end = end;

	D3DXVECTOR3 position[] = { start, end };
	UINT lineCount = 2;

	selectAxis[0] = selectAxis[1] = selectAxis[2] = false;
	length = 1.0f;
	
	__super::Init(position, lineCount);
}

DebugDrawLine::~DebugDrawLine()
{
}

void DebugDrawLine::Render()
{
	__super::Render();
}

void DebugDrawLine::PostRender()
{
	__super::PostRender();

	float s[] = { start.x, start.y,start.z };
	float e[] = { end.x,end.y,end.z };

	ImGui::ColorEdit4("Color", (float*)colorBuffer->Data.Color);

	if (ImGui::DragFloat3("Start Pos", s) ||
		ImGui::DragFloat3("End Pos", e))
		SetPosition(D3DXVECTOR3(s), D3DXVECTOR3(e));

	ImGui::Checkbox("X Axis", &selectAxis[0]);
	ImGui::SameLine(90);
	ImGui::Checkbox("Y Axis", &selectAxis[1]);
	ImGui::SameLine(180);
	ImGui::Checkbox("Z Axis", &selectAxis[2]);

	ImGui::DragFloat("Length", &length, 0.1f);
	if (ImGui::Button("Draw"))
	{
		if (selectAxis[0])
			end.x = start.x + length;
		else
			end.x = start.x;

		if (selectAxis[1])
			end.y = start.y + length;
		else
			end.y = start.y;

		if (selectAxis[2])
			end.z = start.z + length;
		else
			end.z = start.z;

		SetPosition(start, end);
	}
}

void DebugDrawLine::SetPosition(D3DXVECTOR3 & start, D3DXVECTOR3 & end)
{
	this->start = start;
	this->end = end;

	vertices[0].Position = start;
	vertices[1].Position = end;

	UpdateBuffer();
}
