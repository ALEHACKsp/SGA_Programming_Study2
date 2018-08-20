#include "stdafx.h"
#include "DrawLandScape.h"

#include "../LandScape/Terrain.h"
#include "../Objects/MeshSphere.h"

DrawLandScape::DrawLandScape(ExecuteValues * values)
	:Execute(values)
{
	// Create Terrain
	{
		wstring heightMap = Contents + L"HeightMaps/HeightMap256.png";
		Material * material = new Material(Shaders + L"013_TerrainBrush.hlsl");

		material->SetDiffuseMap(Textures + L"Dirt2.png");

		terrain = new Terrain(values, material, heightMap);
	}
}

DrawLandScape::~DrawLandScape()
{
	SAFE_DELETE(terrain);
}

void DrawLandScape::Update()
{
	terrain->Update();
}

void DrawLandScape::PreRender()
{

}

void DrawLandScape::Render()
{
	terrain->Render();
}

void DrawLandScape::PostRender()
{

}

void DrawLandScape::ResizeScreen()
{
}

