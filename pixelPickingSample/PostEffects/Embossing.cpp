#include "stdafx.h"
#include "Embossing.h"

Embossing::Embossing(ExecuteValues * values)
	: Render2D(values, Shaders + L"032_Embossing.hlsl")
{
	buffer = new Buffer();
}

Embossing::~Embossing()
{
	SAFE_DELETE(buffer);
}

void Embossing::Render()
{
	ImGui::Separator();
	ImGui::SliderInt("Embossing PixelOffset", &buffer->Data.PixelOffset, 1, 10);
	ImGui::Separator();

	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;

	buffer->SetPSBuffer(10);

	__super::Render();
}
