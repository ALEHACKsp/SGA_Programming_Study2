#include "stdafx.h"
#include "Mosaic.h"

Mosaic::Mosaic(ExecuteValues * values)
	: Render2D(values, Shaders + L"Homework/Mosaic.hlsl")
{
	buffer = new Buffer();

}

Mosaic::~Mosaic()
{
	SAFE_DELETE(buffer);
}

void Mosaic::Render()
{
	ImGui::Separator();
	ImGui::SliderInt("Mosaic Count", &buffer->Data.Count, 0, 20);
	ImGui::Separator();

	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;

	buffer->SetPSBuffer(10);

	__super::Render();
}
