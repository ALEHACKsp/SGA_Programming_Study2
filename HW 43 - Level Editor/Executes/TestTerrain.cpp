#include "stdafx.h"
#include "TestTerrain.h"

#include "Environment\CubeSky.h"
#include "Environment\Terrain.h"
#include "Environment\PathFinding.h"

#include "Environment\HeightMap.h"

TestTerrain::TestTerrain()
{
	sky = new CubeSky(Textures + L"sunsetcube1024.dds");

	// Create Terrain
	{
		Terrain::InitDesc desc =
		{
			Textures + L"Terrain/terrain.raw",
			50.0f,
			2049, 2049,
			
			// LayerMapFile[5]
			{
				//Textures + L"Dirt3.png",
				Textures + L"Terrain/grass.png",
				Textures + L"Terrain/darkdirt.png",
				Textures + L"Terrain/stone.png",
				Textures + L"Terrain/lightdirt.png",
				Textures + L"Terrain/snow.png",
			},
			Textures + L"Terrain/blend.png",

			0.5f
		};
		desc.material = new Material(Shaders + L"061_Terrain.fx");

		terrain = new Terrain(desc);
		//pathFinding = new PathFinding(terrain);
	}
}

TestTerrain::~TestTerrain()
{
	SAFE_DELETE(sky);

	SAFE_DELETE(terrain);
}

void TestTerrain::Update()
{
	//sky->Update();
	terrain->Update();
}

void TestTerrain::PreRender()
{
	terrain->PreRender();
}

void TestTerrain::Render()
{
	//sky->Render();
	terrain->Render();
}

void TestTerrain::PostRender()
{

}