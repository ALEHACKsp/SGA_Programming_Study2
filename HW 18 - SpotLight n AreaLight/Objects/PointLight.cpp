#include "stdafx.h"
#include "PointLight.h"
#include "../Renders/Lines/DebugDrawSphere.h"

PointLight::PointLight()
{
	buffer = new Buffer();
	D3DXVECTOR3 pos = { 0,0,0 };
	float range = 1.0f;
	debug = new DebugDrawSphere(pos, range);
}

PointLight::~PointLight()
{
	SAFE_DELETE(debug);
	SAFE_DELETE(buffer);
}

void PointLight::Update()
{
	buffer->SetPSBuffer(2);
}

void PointLight::PostRender(int index)
{
	if (index >= buffer->Data.Count) return;
	ImGui::Text(("PointLight " + to_string(index)).c_str());

	ImGui::DragFloat3("Position", (float*)&buffer->Data.Lights[index].Position);
	ImGui::DragFloat("Range", &buffer->Data.Lights[index].Range, 0.1f, 1.0f);
	ImGui::DragFloat("Intensity", &buffer->Data.Lights[index].Intensity, 0.1f, 0.01f);
	ImGui::ColorEdit3("Color", (float*)&buffer->Data.Lights[index].Color);

	DebugDraw(index);
}

void PointLight::DebugDraw(int index)
{
	if (index >= buffer->Data.Count) return;

	Desc desc = this->buffer->Data.Lights[index];
	debug->Set(desc.Position, desc.Range,
		D3DXCOLOR(desc.Color.x, desc.Color.y, desc.Color.z, 1.0f));
	debug->Render();
}
