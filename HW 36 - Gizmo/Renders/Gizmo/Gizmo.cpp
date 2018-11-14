#include "stdafx.h"
#include "Gizmo.h"

#include "Renders\Gizmo\GizmoAxis.h"

Gizmo::Gizmo(ExecuteValues* values)
	: values(values)
	,lineOffset(1.0f), lineLength(3.0f), singleAxisThickness(0.2f)
{
	halfLineOffset = lineOffset / 2;

	xAxisBox = new BBox(
		D3DXVECTOR3(lineOffset, 0, 0),
		D3DXVECTOR3(lineOffset + lineLength, singleAxisThickness, singleAxisThickness));
	yAxisBox = new BBox(
		D3DXVECTOR3(0, lineOffset, 0),
		D3DXVECTOR3(singleAxisThickness, lineOffset + lineLength, singleAxisThickness));
	zAxisBox = new BBox(
		D3DXVECTOR3(0, 0, lineOffset),
		D3DXVECTOR3(singleAxisThickness, singleAxisThickness, lineOffset + lineLength));

	for (int i = 0; i < 3; i++)
		axis[i] = new DebugLine();

	axiss.push_back(new GizmoAxis(GizmoAxis::Kind::X, lineOffset, halfLineOffset, lineLength));
	axiss.push_back(new GizmoAxis(GizmoAxis::Kind::Y, lineOffset, halfLineOffset, lineLength));
	axiss.push_back(new GizmoAxis(GizmoAxis::Kind::Z, lineOffset, halfLineOffset, lineLength));

	axisLine = new DebugLine();
	UpdateLine();

	axisColor[(int)Axis::X] = D3DXCOLOR(1, 0, 0, 1);
	axisColor[(int)Axis::Y] = D3DXCOLOR(0, 1, 0, 1);
	axisColor[(int)Axis::Z] = D3DXCOLOR(0, 0, 1, 1);
	highlightColor = D3DXCOLOR(1, 1, 0, 1);

	visible = false;
	enable = false;

	depthMode[0] = new DepthStencilState();
	depthMode[1] = new DepthStencilState();
	depthMode[1]->DepthEnable(false);

	render = NULL;
}

Gizmo::~Gizmo()
{
	SAFE_DELETE(xAxisBox);
	SAFE_DELETE(yAxisBox);
	SAFE_DELETE(zAxisBox);

	SAFE_DELETE(axisLine);
	for (ILine* line : axiss)
		SAFE_DELETE(line);

	SAFE_DELETE(depthMode[0]);
	SAFE_DELETE(depthMode[1]);

	for (int i = 0; i < 3; i++)
		SAFE_DELETE(axis[i]);
}

void Gizmo::Set(GameRender* render)
{
	this->render = render;

	Position(render->Position());

	UpdateLine();
}

void Gizmo::Update(Ray * ray)
{
	float result = 0;
	if (Mouse::Get()->Down(0)) {
		D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();

		D3DXVECTOR3 pos = Position();

		float speed = 20;

		if (xAxisBox->Intersect(ray, result)) {
			pos.x += val.x * speed * Time::Delta();
		}
		else if (yAxisBox->Intersect(ray, result)) {
			pos.y += val.x * speed * Time::Delta();
		}
		else if (zAxisBox->Intersect(ray, result)) {
			pos.z += val.x * speed * Time::Delta();
		}

		Position(pos);
		render->Position(pos);
		UpdateLine();
	}
}

void Gizmo::UpdateLine()
{
	axisLine->Draw(World(), axiss);

	axis[0]->Draw(World(), xAxisBox);
	axis[1]->Draw(World(), yAxisBox);
	axis[2]->Draw(World(), zAxisBox);
}

void Gizmo::Render()
{
	if (visible) {
		if (render != NULL) {
			ImGui::Begin("Gizmo");

			D3DXVECTOR3 pos, rot, scale;
			pos = render->Position();
			rot = render->Rotation();
			scale = render->Scale();

			if (ImGui::DragFloat3("Position", (float*)&pos, 0.1f)) {
				render->Position(pos);
				Position(pos);
				UpdateLine();
			}
			if (ImGui::DragFloat3("Rotation", (float*)&rot, 0.1f)) {
				render->RotationDegree(rot);
				//UpdateLine();
			}
			if (ImGui::DragFloat3("Scale", (float*)&scale, 0.1f)) {
				render->Scale(scale);
				//UpdateLine();
			}

			ImGui::End();
		}

		depthMode[1]->OMSetDepthStencilState();
		// axis
		{
			for (int i = 0; i < 3; i++)
				axis[i]->Render();

			// X
			axisLine->Color(axisColor[(int)Axis::X]);
			axisLine->Render(6, 0);
			// Y
			axisLine->Color(axisColor[(int)Axis::Y]);
			axisLine->Render(6, 6);
			// Z
			axisLine->Color(axisColor[(int)Axis::Z]);
			axisLine->Render(6, 12);
		}
		depthMode[0]->OMSetDepthStencilState();
	}
}
