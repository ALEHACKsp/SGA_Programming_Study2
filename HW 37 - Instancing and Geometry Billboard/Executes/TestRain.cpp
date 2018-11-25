#include "stdafx.h"
#include "TestRain.h"

#include "../Environment/Rain.h"
#include "../Environment/OldRain.h"

TestRain::TestRain()
{
	rain = new Rain(D3DXVECTOR3(256, 100, 256), 1000);
	rain->Position(D3DXVECTOR3(128, 0, 128));
	rain2 = new OldRain(D3DXVECTOR3(256, 100, 256), 1000);
	rain2->Position(D3DXVECTOR3(128, 0, 128));

	bBillboardInstancing = false;
}

TestRain::~TestRain()
{
	SAFE_DELETE(rain);
	SAFE_DELETE(rain2);
}

void TestRain::Update()
{
	rain->Update();
	rain2->Update();
}

void TestRain::PreRender()
{

}

void TestRain::Render()
{
	ImGui::Checkbox("Billboard n Instancing", &bBillboardInstancing);

	if(bBillboardInstancing)
		rain->Render();
	else
		rain2->Render();
}

void TestRain::PostRender()
{

}
