#include "stdafx.h"
#include "PostEffects.h"

PostEffects::PostEffects(ExecuteValues * values)
	: Render2D(values, Shaders + L"Homework/PostEffects.hlsl")
{
	buffer = new Buffer();

}

PostEffects::~PostEffects()
{
	SAFE_DELETE(buffer);
}

void PostEffects::Render()
{
	ImGui::RadioButton("Grayscale1", &buffer->Data.Select, 0);
	ImGui::SameLine(120);
	ImGui::RadioButton("Grayscale2", &buffer->Data.Select, 1);
	ImGui::RadioButton("Inverse", &buffer->Data.Select, 2);
	ImGui::SameLine(120);
	ImGui::RadioButton("Blur", &buffer->Data.Select, 3);
	ImGui::RadioButton("Mosaic", &buffer->Data.Select, 4);
	ImGui::RadioButton("Gamma", &buffer->Data.Select, 5);
	ImGui::RadioButton("Bit Planner Slicing", &buffer->Data.Select, 6);
	ImGui::RadioButton("Embossing", &buffer->Data.Select, 7);
	ImGui::RadioButton("Sharpening1", &buffer->Data.Select, 8);
	ImGui::SameLine(120);
	ImGui::RadioButton("Sharpening2", &buffer->Data.Select, 9);

	ImGui::Separator();
	if(buffer->Data.Select == 3 || buffer->Data.Select == 4 
		|| buffer->Data.Select == 7 || buffer->Data.Select == 9)
		ImGui::SliderInt("Count", &buffer->Data.Count, 0, 20);
	if (buffer->Data.Select == 5)
		ImGui::ColorEdit4("Gamma", (float*)&buffer->Data.Gamma);
	if (buffer->Data.Select == 6)
		ImGui::SliderInt("Slicing Bit", &buffer->Data.Count, 0, 7);

	ImGui::Separator();

	D3DXVECTOR2 scale = Scale();
	buffer->Data.Width = (int)scale.x;
	buffer->Data.Height = (int)scale.y;

	buffer->SetPSBuffer(10);

	__super::Render();
}
