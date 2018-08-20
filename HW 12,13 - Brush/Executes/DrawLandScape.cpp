#include "stdafx.h"
#include "DrawLandScape.h"

#include "../LandScape/Terrain.h"
#include "../Objects/MeshSphere.h"

DrawLandScape::DrawLandScape(ExecuteValues * values)
	:Execute(values)
{
	// Create Terrain
	{
		Material * material = new Material(Shaders + L"Homework/Brush.hlsl");

		material->SetDiffuseMap(Textures + L"Dirt2.png");

		terrain = new Terrain(values, material);
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
	terrain->PostRender();
}

void DrawLandScape::ResizeScreen()
{
}

