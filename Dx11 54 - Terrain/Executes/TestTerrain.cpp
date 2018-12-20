#include "stdafx.h"
#include "TestTerrain.h"

#include "./Environment/Terrain.h"

TestTerrain::TestTerrain()
{
	material = new Material(Shaders + L"060_Terrain.fx");
	material->SetDiffuseMap(Textures + L"Dirt.png");

	wstring heightMap = Contents + L"HeightMaps/HeightMap256.png";

	terrain = new Terrain(material, heightMap);
}

TestTerrain::~TestTerrain()
{
	SAFE_DELETE(material);
	SAFE_DELETE(terrain);
}

void TestTerrain::Update()
{

}

void TestTerrain::PreRender()
{
	terrain->Update();
}

void TestTerrain::Render()
{
	terrain->Render();
}

void TestTerrain::PostRender()
{

}