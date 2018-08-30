#include "stdafx.h"
#include "SpotLight.h"

#include "../Renders/Lines/DebugDrawCone.h"

SpotLight::SpotLight()
{
	buffer = new Buffer();

	D3DXVECTOR3 center = { 0,0,0 };
	float radius = 3.0f;
	float height = 5.0f;

	debug = new DebugDrawCone(center,radius,height);
}

SpotLight::~SpotLight()
{
	SAFE_DELETE(debug);
	SAFE_DELETE(buffer);
}

void SpotLight::Update()
{
	buffer->SetPSBuffer(3);
}

void SpotLight::PostRender(int index)
{
	if (index >= buffer->Data.Count) return;
	ImGui::Text(("SpotLight " + to_string(index)).c_str());

	ImGui::DragFloat3("Position", (float*)&buffer->Data.Lights[index].Position);
	ImGui::DragFloat3("Direction", (float*)&buffer->Data.Lights[index].Direction, 0.1f, -1.0f, 1.0f);
	ImGui::DragFloat("InnerAngle", &buffer->Data.Lights[index].InnerAngle, 0.001f, 0.0f);
	ImGui::DragFloat("OuterAngle", &buffer->Data.Lights[index].OuterAngle, 0.001f, 0.0f);
	ImGui::ColorEdit3("Color", (float*)&buffer->Data.Lights[index].Color);

	//DebugDraw(index);
}

void SpotLight::DebugDraw(int index)
{
	if (index >= buffer->Data.Count) return;

	Desc desc = this->buffer->Data.Lights[index];

	float radius = 5.0f;
	float height = 10.0f;

	D3DXVECTOR3 center = desc.Position + desc.Direction * height;

	debug->Set(center, radius, height,
		D3DXCOLOR(desc.Color.x, desc.Color.y, desc.Color.z, 1.0f));
	debug->Render();
}
