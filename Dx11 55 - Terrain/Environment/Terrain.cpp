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

void Terrain::Render()
{
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

#pragma region PathFinding
void Terrain::InitTileMap()
{
	ResetTileMap();
	SetTilePositionAndTypes();
	ConnectNeighboringTiles();
	CreateTileSets();
}

void Terrain::ResetTileMap()
{
}

void Terrain::SetTilePositionAndTypes()
{
}

void Terrain::ConnectNeighboringTiles()
{
}

void Terrain::CreateTileSets()
{
}
#pragma endregion


