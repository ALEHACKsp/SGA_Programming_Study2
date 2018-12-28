#include "stdafx.h"
#include "Sharpening.h"

Sharpening::Sharpening(ExecuteValues * values)
	: Render2D(values, Shaders + L"032_Sharpening.hlsl")
{
	buffer = new Buffer();
}

Sharpening::~Sharpening()
{
	SAFE_DELETE(buffer);
}

void Sharpening::Render()
{
	ImGui::Separator();
	ImGui::SliderFloat("Sharpening PixelOffset", &buffer->Data.Sharpen, 1, 20);
	ImGui::Separator();

	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;

	buffer->SetPSBuffer(10);

	__super::Render();
}
