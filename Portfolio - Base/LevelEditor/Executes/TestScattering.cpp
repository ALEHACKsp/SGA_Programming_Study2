#include "stdafx.h"
#include "TestScattering.h"

#include "Environment\ScatteringSky.h"

void TestScattering::Initialize()
{
	sky = new ScatteringSky();
	sky->Initialize();
}

void TestScattering::Ready()
{
	sky->Ready();
}

void TestScattering::Destroy()
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

void TestScattering::ResizeScreen()
{
}
