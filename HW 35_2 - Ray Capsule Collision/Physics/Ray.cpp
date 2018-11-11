#include "stdafx.h"
#include "Ray.h"

#include "Renders\Lines\DebugDrawLine.h"

Ray::Ray(D3DXVECTOR3& origin, D3DXVECTOR3 direction, float length)
	:origin(origin), direction(direction), length(length)
{
	D3DXVECTOR3 end = origin + direction * length;

	debug = new DebugDrawLine(origin, end);
}

Ray::~Ray()
{
	SAFE_DELETE(debug);
}

void Ray::Render()
{
	debug->Render();
}

void Ray::PostRender()
{
	D3DXCOLOR color = debug->GetColor();
	if (ImGui::ColorEdit4("Color##Ray", (float*)&color))
		debug->SetColor(color);

	bool bCheck = ImGui::DragFloat3("Origin", (float*)&origin, 0.1f);
	bCheck |= ImGui::DragFloat3("Direction", (float*)&direction, 0.01f);
	bCheck |= ImGui::DragFloat("Length", &length, 0.1f);

	if (bCheck)
	{
		D3DXVECTOR3 end = origin + direction * length;
		debug->Set(origin, end);
	}
}
