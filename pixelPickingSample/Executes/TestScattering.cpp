#include "stdafx.h"
#include "TestScattering.h"
#include "./Landscape/Sky.h"
#include "./Landscape/TerrainRender.h"
#include "./Landscape/Cloud.h"

TestScattering::TestScattering(ExecuteValues * values)
	: Execute(values)
{
	sky = new Sky(values);
	cloud = new Cloud(values, CloudType::SpotClouds);
	terrain = new TerrainRender();
}

TestScattering::~TestScattering()
{
	SAFE_DELETE(sky);
	SAFE_DELETE(cloud);
	SAFE_DELETE(terrain);
}

void TestScattering::Update()
{

	sky->Update();
	cloud->Update();
	terrain->Update();
}

void TestScattering::PreRender()
{
	sky->PreRender();
}

void TestScattering::Render()
{
	sky->Render();
	cloud->Render();
	terrain->Render();
}

void TestScattering::PostRender()
{	
}