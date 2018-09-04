#include "stdafx.h"
#include "DrawLandScape.h"

#include "../LandScape/Sky.h"
#include "../LandScape/Terrain.h"
#include "../LandScape/QuadTree.h"

#include "../Objects/MeshSphere.h"
#include "../Objects/MeshCube.h"
#include "../Objects/Billboard.h"
#include "../Objects/PointLight.h"
#include "../Objects/SpotLight.h"

DrawLandScape::DrawLandScape(ExecuteValues * values)
	:Execute(values)
{
	// Create Sky
	{
		sky = new Sky(values);
	}

	// Create Terrain
	{
		wstring heightMap = Contents + L"HeightMaps/HeightMap256.png";
		Material * material = new Material(Shaders + L"016_TerrainBrushLine.hlsl");

		terrain = new Terrain(values, material);
	}

	// Cretaet QuadTree
	{
		quadTree = new QuadTree(values, terrain);
	}
}

DrawLandScape::~DrawLandScape()
{
	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);

	SAFE_DELETE(quadTree);
}

void DrawLandScape::Update()
{
	sky->Update();
	//terrain->Update();

	quadTree->Update();
}

void DrawLandScape::PreRender()
{

}

void DrawLandScape::Render()
{
	sky->Render();
	//terrain->Render();

	quadTree->Render();
}

void DrawLandScape::PostRender()
{
}

void DrawLandScape::ResizeScreen()
{
}

