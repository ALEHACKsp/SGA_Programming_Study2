#include "stdafx.h"
#include "ExDebugDraw.h"

#include "LandScape\Terrain.h"

#include "Renders\Lines\DebugDrawLine.h"
#include "Renders\Lines\DebugDrawCube.h"
#include "Renders\Lines\DebugDrawSphere.h"
#include "Renders\Lines\DebugDrawSphere2.h"
#include "Renders\Lines\DebugDrawCone.h"
#include "Renders\Lines\DebugDrawCone2.h"
#include "Renders\Lines\DebugDrawCapsule.h"

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
				D3DXVECTOR3 start = { 0,0,0 };
				D3DXVECTOR3 end = start + D3DXVECTOR3(1,1,1);
				lines.push_back(new DebugDrawLine(start, end));
				lines.back()->Position(pickPos);
			}
			if (ImGui::MenuItem("Cube", "Create Debug Cube"))
			{
				D3DXVECTOR3 halfSize(1, 1, 1);
				lines.push_back(new DebugDrawCube(halfSize));
				lines.back()->Position(pickPos.x, 2.0f, pickPos.z);
			}
			if (ImGui::MenuItem("Sphere", "Create Debug Sphere"))
			{
				float radius = 5.0f;
				lines.push_back(new DebugDrawSphere(radius));
				lines.back()->Position(pickPos.x, 6.0f, pickPos.z);
			}
			if (ImGui::MenuItem("Sphere2", "Create Debug Sphere2"))
			{
				float radius = 5.0f;
				lines.push_back(new DebugDrawSphere2(radius));
				lines.back()->Position(pickPos.x, 6.0f, pickPos.z);
			}
			if (ImGui::MenuItem("Cone", "Create Debug Cone"))
			{
				D3DXVECTOR3 center = { 0,0,0 };
				float height = 10.0f;
				lines.push_back(new DebugDrawCone(height));
				lines.back()->Position(pickPos.x, 11, pickPos.z);
			}
			if (ImGui::MenuItem("Cone2", "Create Debug Cone2"))
			{
				float radius = 5.0f;
				float height = 10.0f;
				lines.push_back(new DebugDrawCone2(radius, height));
				lines.back()->Position(pickPos.x, 11, pickPos.z);
			}
			if (ImGui::MenuItem("Capsule", "Create Debug Capsule"))
			{
				float radius = 5.0f;
				float height = 10.0f;
				lines.push_back(new DebugDrawCapsule(radius, height));
				lines.back()->Position(pickPos.x, 11, pickPos.z);
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

