#include "stdafx.h"
#include "DrawLandscape.h"
#include "./MapTool/MapTool.h"
#include "./Landscape/Sky.h"
#include "./Landscape/Terrain.h"
#include "./Landscape/QuadTree.h"
#include "./Draw/MeshCube.h"

DrawLandscape::DrawLandscape(ExecuteValues * values)
	: Execute(values)
{
	sky = new Sky(values);

	renderTarget = new RenderTarget(values);
	postEffect = new Render2D(values);
	
	//Create Terrain
	{
		Material* material = new Material(Shaders + L"030_Terrain.hlsl");
		terrain = new Terrain(Contents + L"HeightMaps/HeightMap256.png");
	}

	quadTree = new QuadTree(values, terrain);
}

DrawLandscape::~DrawLandscape()
{
	SAFE_DELETE(quadTree);
	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);
}

void DrawLandscape::Update()
{
	sky->Update();
	quadTree->Update();
}

void DrawLandscape::PreRender()
{
	renderTarget->Set();
	sky->Render();
	quadTree->Render();
}

void DrawLandscape::Render()
{
}

void DrawLandscape::PostRender()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	postEffect->Position(0, 0);
	postEffect->Scale(desc.Width, desc.Height);
	postEffect->SRV(renderTarget->GetSRV());
	postEffect->Update();
	postEffect->Render();
}

void DrawLandscape::ResizeScreen()
{
}
