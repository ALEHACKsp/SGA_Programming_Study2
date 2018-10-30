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

	cullMode[0] = new RasterizerState();
	cullMode[1] = new RasterizerState();
	cullMode[1]->FrontCounterClockwise(true);

	depthMode[0] = new DepthStencilState();
	depthMode[1] = new DepthStencilState();
	depthMode[1]->DepthEnable(false);
}

Sky::~Sky()
{
	SAFE_DELETE(buffer);

	SAFE_DELETE(model);

	SAFE_DELETE(cullMode[0]);
	SAFE_DELETE(cullMode[1]);
	
	SAFE_DELETE(depthMode[0]);
	SAFE_DELETE(depthMode[1]);
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
	ImGui::Begin("Sky");
	ImGui::Separator();

	ImGui::SliderFloat3("Apex", (float *)buffer->Data.Apex, 0, 1);
	ImGui::SliderFloat3("Center", (float *)buffer->Data.Center, 0, 1);
	ImGui::SliderFloat("Height", &buffer->Data.Height, 0, 10);
	ImGui::End();

	buffer->SetPSBuffer(10);

	depthMode[1]->OMSetDepthStencilState();
	cullMode[1]->RSSetState();
	{
		model->Render();
	}
	depthMode[0]->OMSetDepthStencilState();
	cullMode[0]->RSSetState();
}
