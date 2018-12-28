#include "stdafx.h"
#include "Gamma.h"

Gamma::Gamma(ExecuteValues * values)
	: Render2D(values, Shaders + L"032_Gamma.hlsl")
{
	buffer = new Buffer();
}

Gamma::~Gamma()
{
	SAFE_DELETE(buffer);
}

void Gamma::Render()
{
	ImGui::Separator();
	ImGui::SliderFloat("Gamma R", &buffer->Data.R, 1, 30);
	ImGui::SliderFloat("Gamma G", &buffer->Data.G, 1, 30);
	ImGui::SliderFloat("Gamma B", &buffer->Data.B, 1, 30);
	ImGui::Separator();

	buffer->SetPSBuffer(10);

	__super::Render();
}
