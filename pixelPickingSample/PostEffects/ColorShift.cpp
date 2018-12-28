#include "stdafx.h"
#include "ColorShift.h"

ColorShift::ColorShift(ExecuteValues * values)
	: Render2D(values, Shaders + L"032_ColorShift.hlsl")
{
	buffer = new Buffer();
}

ColorShift::~ColorShift()
{
	SAFE_DELETE(buffer);
}

void ColorShift::Render()
{
	ImGui::Separator();
	ImGui::ColorEdit3("ColorShift Select", (float*)&buffer->Data.Color);
	ImGui::Separator();

	buffer->SetPSBuffer(10);

	__super::Render();
}
