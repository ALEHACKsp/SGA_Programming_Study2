#include "stdafx.h"
#include "Terrain.h"

#include "HeightMap.h"
#include "TerrainRender.h"

const UINT Terrain::CellsPerPatch = 64;

Terrain::Terrain(InitDesc & desc)
	: desc(desc)
{
	render = new TerrainRender(desc.material, this);
	
	heightMap = new HeightMap(desc.HeightMapWidth, desc.HeightMapHeight, desc.HeightScale);
	heightMap->Load(desc.HeightMap);

	render->Initialize();
}

Terrain::~Terrain()
{
	SAFE_DELETE(render);
	SAFE_DELETE(heightMap);

	SAFE_DELETE(desc.material);
}

void Terrain::Update()
{

}

void Terrain::PreRender()
{
	render->PreRender();
}

void Terrain::Render()
{
	D3DXVECTOR3 pos;
	render->Pick(&pos);

	ImGui::Text("%f %f %f", pos.x, pos.y, pos.z);

	render->Render();
}

void Terrain::Data(UINT row, UINT col, float data)
{
	heightMap->Data(row, col, data);
}

float Terrain::Data(UINT row, UINT col)
{
	return heightMap->Data(row, col);
}

float Terrain::Width()
{
	return (desc.HeightMapWidth - 1) * desc.CellSpacing;
}

float Terrain::Depth()
{
	return (desc.HeightMapHeight - 1) * desc.CellSpacing;
}

float Terrain::GetHeight(float x, float z)
{
	float c = (x + 0.5f * Width()) / desc.CellSpacing;
	float d = (z - 0.5f * Depth()) / desc.CellSpacing;
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	float h00 = heightMap->Data(row, col);
	float h01 = heightMap->Data(row, col + 1);
	float h10 = heightMap->Data(row + 1, col);
	float h11 = heightMap->Data(row + 1, col + 1);

	float s = c - col;
	float t = d - row;

	if (s + t <= 1.0f) 
	{
		float uy = h01 - h00;
		float vy = h01 - h11;
		return h00 + (1.0f - s) * uy + (1.0f - t) * vy;
	}
	else
	{
		float uy = h10 - h11;
		float vy = h01 - h11;
		return h11 + (1.0f - s) * uy + (1.0f - t) * vy;
	}
}



