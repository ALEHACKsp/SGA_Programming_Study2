#include "stdafx.h"
#include "DrawLandScape.h"

#include "../LandScape/Terrain.h"
#include "../Objects/GameTank.h"

DrawLandScape::DrawLandScape(ExecuteValues * values)
	:Execute(values)
{
	// Create Terrain
	{
		wstring heightMap = Contents + L"HeightMaps/HeightMap256.png";
		Material * material = new Material(Shaders + L"011_Terrain.hlsl");

		material->SetDiffuseMap(Textures + L"Dirt2.png");

		terrain = new Terrain(material, heightMap);
	}

	// Create Tank
	{
		tank = new GameTank(Models + L"Tank/", L"Tank.material");

		D3DXMATRIX S;
		D3DXMatrixScaling(&S, 0.5f, 0.5f, 0.5f);
		tank->RootAxis(S);

		tank->Position(D3DXVECTOR3(80, 10, 30));
	}
}

DrawLandScape::~DrawLandScape()
{
	SAFE_DELETE(terrain);
	SAFE_DELETE(tank);
}

void DrawLandScape::Update()
{
	terrain->Update();
	tank->Update();

	terrain->UpdateModel(tank);
}

void DrawLandScape::PreRender()
{

}

void DrawLandScape::Render()
{
	terrain->Render();
	tank->Render();
}

void DrawLandScape::PostRender()
{
	terrain->PostRender();
	
	ImGui::Begin("Model");
	{
		ImGui::Text("Tank Info");
		tank->PostRender();
	}
	ImGui::End();
}

void DrawLandScape::ResizeScreen()
{
}

