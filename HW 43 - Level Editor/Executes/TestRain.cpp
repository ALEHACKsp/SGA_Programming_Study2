#include "stdafx.h"
#include "TestRain.h"

#include "../Environment/Rain.h"

TestRain::TestRain()
{
	rain = new Rain(D3DXVECTOR3(256, 100, 256), 1000);
	rain->Position(D3DXVECTOR3(128, 0, 128));
}

TestRain::~TestRain()
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
