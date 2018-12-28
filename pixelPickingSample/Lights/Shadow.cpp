#include "stdafx.h"
#include "Shadow.h"
#include "./Viewer/Fixity.h"
#include "./Objects/GameModel.h"

Shadow::Shadow(ExecuteValues * values)
	: values(values)
{
	shaderDepth = new Shader(Shaders + L"043_Shadow.hlsl", "VS_Depth", "PS_Depth");
	shaderRender = new Shader(Shaders + L"043_Shadow.hlsl", "VS_Shadow", "PS_Shadow");

	//크기는 화면의 두배가 좋음
	depthShadow = new RenderTarget(values, (UINT)values->Viewport->GetWidth() * 2, (UINT)values->Viewport->GetHeight() * 2);
	depthRender = new Render2D(values);
	depthRender->Position(0, 0);
	depthRender->Scale(200, 200);

	//Perspective
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		perspective = new Perspective(1, 1, Math::PI * 0.5f, 1, 100);
	}

	vpBuffer = new ViewProjectionBuffer();

	samplerState[0] = new SamplerState();
	samplerState[1] = new SamplerState();
	samplerState[1]->ComparisonFunc(D3D11_COMPARISON_LESS_EQUAL);
	samplerState[1]->AddressU(D3D11_TEXTURE_ADDRESS_MIRROR);
	samplerState[1]->AddressV(D3D11_TEXTURE_ADDRESS_MIRROR);
	samplerState[1]->Filter(D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);

	buffer = new Buffer();
	buffer->Data.Size = D3DXVECTOR2(2048, 2048);
}

Shadow::~Shadow()
{
	SAFE_DELETE(vpBuffer);
	SAFE_DELETE(shaderRender);
	SAFE_DELETE(shaderDepth);
	SAFE_DELETE(perspective);
}

void Shadow::Add(GameModel * obj)
{
	objs.push_back(obj);
}

void Shadow::Update()
{
	depthRender->Update();
}

void Shadow::PreRender()
{
	depthShadow->Set();

	D3DXMATRIX V, P;
	D3DXMatrixLookAtLH(&V, &values->GlobalLight->Data.Position, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 1, 0));
	values->ViewProjection->SetView(V);

	perspective->GetMatrix(&P);
	values->ViewProjection->SetProjection(P);
	values->ViewProjection->SetVSBuffer(0);

	vpBuffer->SetView(V);
	vpBuffer->SetProjection(P);

	for (GameModel* obj : objs)
	{
		obj->Update();
		obj->SetShader(shaderDepth);
		obj->Render();
	}
}

void Shadow::Render()
{
	ImGui::SliderInt("Shadow Selected", &buffer->Data.Selected, 0, 2);
	ImGui::SliderFloat("Shadow Bias", &buffer->Data.Bias, 0.0001f, 0.1f, "%.5f", 0.0001f);
	ImGui::SliderFloat3("Light Position", (float*)&values->GlobalLight->Data.Position, -100, 100);

	vpBuffer->SetVSBuffer(10);
	buffer->SetPSBuffer(10);

	ID3D11ShaderResourceView* view = depthShadow->GetSRV();
	D3D::GetDC()->PSSetShaderResources(10, 1, &view);

	samplerState[1]->PSSetSamplers(10);
	for (GameModel* obj : objs)
	{
		obj->SetShader(shaderRender);
		obj->Render();
	}
	samplerState[0]->PSSetSamplers(10);

	depthRender->SRV(depthShadow->GetSRV());
	depthRender->Update();
	depthRender->Render();
}
