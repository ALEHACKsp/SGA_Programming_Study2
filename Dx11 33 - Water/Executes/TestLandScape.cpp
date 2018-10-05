#include "stdafx.h"
#include "TestLandScape.h"

#include "../LandScape/Sky.h"
#include "../LandScape/TerrainRender.h"
#include "../LandScape/Water.h"


TestLandScape::TestLandScape(ExecuteValues * values)
	:Execute(values)
{
	sky = new Sky(values);
	terrain = new TerrainRender();
	water = new Water(values, 64, 64, 8, terrain->HeightMap());
}

TestLandScape::~TestLandScape()
{
	SAFE_DELETE(sky);
	SAFE_DELETE(terrain);
	SAFE_DELETE(water);
}

void TestLandScape::Update()
{
	sky->Update();
	terrain->Update();
	water->Update();
}

void TestLandScape::PreRender()
{

}

void TestLandScape::Render()
{
	sky->Render();
	terrain->Render();
	water->Render();
}

void TestLandScape::PostRender()
{

}

