#include "stdafx.h"
#include "Sky.h"

#include "Objects\GameModel.h"

Sky::Sky(ExecuteValues* values)
	:values(values)
{
	model = new GameModel(Materials + L"Meshes/", L"Sphere.Material",
		Models + L"Meshes/", L"Sphere.mesh");

	for (Material* material : model->GetModel()->Materials())
	{
		material->SetShader(Shaders + L"015_Sky.hlsl");
	}

	buffer = new Buffer();

	// Create Rasterizer
	{
		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);
		States::CreateRasterizer(&desc, &cullMode[0]);

		desc.FrontCounterClockwise = true;
		States::CreateRasterizer(&desc, &cullMode[1]);
	}

	// Create DepthState
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		States::GetDepthStencilDesc(&desc);
		States::CreateDepthStencil(&desc, &depthMode[0]);

		desc.DepthEnable = false;
		States::CreateDepthStencil(&desc, &depthMode[1]);
	}
}

Sky::~Sky()
{
	SAFE_DELETE(buffer);

	SAFE_DELETE(model);

	SAFE_RELEASE(cullMode[0]);
	SAFE_RELEASE(cullMode[1]);
	
	SAFE_RELEASE(depthMode[0]);
	SAFE_RELEASE(depthMode[1]);
}

void Sky::Update()
{
	D3DXVECTOR3 camPos;
	values->MainCamera->Position(&camPos);

	model->Position(camPos);
	model->Update();
}

void Sky::Render()
{
	ImGui::Separator();
	ImGui::Text("Sky");
	ImGui::Separator();

	ImGui::SliderFloat3("Apex", (float *)buffer->Data.Apex, 0, 1);
	ImGui::SliderFloat3("Center", (float *)buffer->Data.Center, 0, 1);
	ImGui::SliderFloat("Height", &buffer->Data.Height, 0, 10);
	ImGui::Separator();

	buffer->SetPSBuffer(10);

	D3D::GetDC()->OMSetDepthStencilState(depthMode[1], 0xFF);
	D3D::GetDC()->RSSetState(cullMode[1]);
	{
		model->Render();
	}
	D3D::GetDC()->RSSetState(cullMode[0]);
	D3D::GetDC()->OMSetDepthStencilState(depthMode[0], 0xFF);
}
