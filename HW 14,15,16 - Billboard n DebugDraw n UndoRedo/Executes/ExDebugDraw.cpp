#include "stdafx.h"
#include "ExDebugDraw.h"

#include "LandScape\Terrain.h"

#include "Renders\Lines\DebugDrawLine.h"
#include "Renders\Lines\DebugDrawCube.h"
#include "Renders\Lines\DebugDrawSphere.h"

ExDebugDraw::ExDebugDraw(ExecuteValues * values)
	:Execute(values)
	, selectLine(0)
	, bPicked(false)
{

}

ExDebugDraw::~ExDebugDraw()
{
	for (Line* line : lines)
		SAFE_DELETE(line);
}

void ExDebugDraw::Update()
{
	if (Mouse::Get()->Down(1))
		bPicked = terrain->Y(&pickPos);
	if (!bPicked)
		pickPos = { 0,0,0 };
}

void ExDebugDraw::PreRender()
{
	
}

void ExDebugDraw::Render()
{
	for (Line* line : lines)
		line->Render();
}

void ExDebugDraw::PostRender()
{
	ImGui::Begin("Debug Draws", 0, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		// Create Line
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Line", "Create Debug Line"))
			{
				D3DXVECTOR3 start = pickPos;
				D3DXVECTOR3 end = start + D3DXVECTOR3(1,1,1);
				lines.push_back(new DebugDrawLine(start, end));
			}
			if (ImGui::MenuItem("Cube", "Create Debug Cube"))
			{
				D3DXVECTOR3 center = pickPos;
				D3DXVECTOR3 halfSize(1, 1, 1);
				center.y += 1.0f;
				lines.push_back(new DebugDrawCube(center, halfSize));
			}
			if (ImGui::MenuItem("Sphere", "Create Debug Sphere"))
			{
				D3DXVECTOR3 center = pickPos;
				float radius = 5.0f;
				center.y += radius;
				lines.push_back(new DebugDrawSphere(center, radius));
			}
			ImGui::EndMenu();
		}
		// Delete Line
		if (ImGui::BeginMenu("Delete"))
		{
			if (ImGui::MenuItem("Delete Last", "Delete Last Line"))
			{
				lines.pop_back();
			}

			if (ImGui::MenuItem("Delete All", "Delete All Line"))
			{
				lines.clear();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Text("Pick Position %.2f, %.2f, %.2f", pickPos.x, pickPos.y, pickPos.z);
	ImGui::Separator();

	int i = 0;
	for (Line * line : lines)
	{
		ImGui::RadioButton(to_string(i).c_str(), &selectLine, i);
		ImGui::SameLine(50);
		ImGui::Text(line->GetName().c_str());

		if(selectLine == i)
			line->PostRender();

		//if (ImGui::Button("Delete"))
		
		ImGui::Separator();
		i++;
	}

	ImGui::End();
}

void ExDebugDraw::ResizeScreen()
{
}

