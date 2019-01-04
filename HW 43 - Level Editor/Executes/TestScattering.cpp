#include "stdafx.h"
#include "TestScattering.h"

#include "Environment\ScatteringSky.h"

TestScattering::TestScattering()
{
	sky = new ScatteringSky();
}

TestScattering::~TestScattering()
{
	SAFE_DELETE(sky);
}

void TestScattering::Update()
{
	sky->Update();
}

void TestScattering::PreRender()
{
	sky->PreRender();
}

void TestScattering::Render()
{
	sky->Render();
}

void TestScattering::PostRender()
{

}