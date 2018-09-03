#include "stdafx.h"
#include "AreaLight.h"

#include "../Renders/Lines/DebugDrawCube.h"

AreaLight::AreaLight()
{
	buffer = new Buffer();

	D3DXVECTOR3 center = { 0,0,0 };
	D3DXVECTOR3 halfSize = { 0,0,0 };

	debug = new DebugDrawCube(center,halfSize);
}

AreaLight::~AreaLight()
{
	SAFE_DELETE(debug);
	SAFE_DELETE(buffer);
}

void AreaLight::Update()
{
	buffer->SetPSBuffer(4);
}

void AreaLight::PostRender(int index)
{
	if (index >= buffer->Data.Count) return;
	ImGui::Text(("AreaLight " + to_string(index)).c_str());

	ImGui::DragFloat3("Position", (float*)&buffer->Data.Lights[index].Position);
	ImGui::DragFloat3("HalfSize", (float*)&buffer->Data.Lights[index].HalfSize);
	ImGui::ColorEdit3("Color", (float*)&buffer->Data.Lights[index].Color);

	DebugDraw(index);
}

void AreaLight::DebugDraw(int index)
{
	if (index >= buffer->Data.Count) return;

	Desc desc = this->buffer->Data.Lights[index];

	D3DXVECTOR3 temp = desc.HalfSize;
	temp.y = -0.1f;

	debug->Set(desc.Position, temp,
		D3DXCOLOR(desc.Color.x, desc.Color.y, desc.Color.z, 1.0f));
	debug->Render();
}
