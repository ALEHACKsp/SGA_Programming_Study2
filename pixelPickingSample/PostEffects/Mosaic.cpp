#include "stdafx.h"
#include "Mosaic.h"

Mosaic::Mosaic(ExecuteValues * values)
	: Render2D(values, Shaders + L"032_Mosaic.hlsl")
{
	buffer = new Buffer();
}

Mosaic::~Mosaic()
{
	SAFE_DELETE(buffer);
}

void Mosaic::Render()
{
	D3DXVECTOR2 scale = Scale();
	ImGui::Separator();
	ImGui::SliderInt("Mosaic Select", &buffer->Data.Count, 1, 30);
	ImGui::Separator();

	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;
	buffer->SetPSBuffer(10);

	__super::Render();
}
