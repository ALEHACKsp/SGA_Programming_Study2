#include "stdafx.h"
#include "TestRain.h"

#include "../Objects/MeshPlane.h"
#include "../LandScape/Rain.h"

TestRain::TestRain(ExecuteValues * values)
	:Execute(values)
{
	planeShader = new Shader(Shaders + L"033_Plane.hlsl");
	plane = new MeshPlane();
	plane->SetDiffuse(0, 0, 1, 1);
	plane->Scale(10, 1, 10);
	plane->SetShader(planeShader);

	rain = new Rain(values, D3DXVECTOR3(100, 100, 100), 500);
}

TestRain::~TestRain()
{
	SAFE_DELETE(plane);
	SAFE_DELETE(planeShader);

	SAFE_DELETE(rain);
}

void TestRain::Update()
{
	rain->Update();
	plane->Update();
}

void TestRain::PreRender()
{

}

void TestRain::Render()
{
	rain->Render();
	plane->Render();
}

void TestRain::PostRender()
{

}
