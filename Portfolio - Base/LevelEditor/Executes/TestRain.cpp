#include "stdafx.h"
#include "TestRain.h"

#include "Environment\Rain.h"

void TestRain::Initialize()
{
	rain = new Rain(D3DXVECTOR3(256, 100, 256), 1000);
	rain->Initialize();
}

void TestRain::Ready()
{
	rain->Ready();
	rain->Position(D3DXVECTOR3(128, 0, 128));
}

void TestRain::Destroy()
{
	SAFE_DELETE(rain);
}

void TestRain::Update()
{
	rain->Update();
}

void TestRain::PreRender()
{

}

void TestRain::Render()
{
	rain->Render();
}

void TestRain::PostRender()
{

}

void TestRain::ResizeScreen()
{
}
