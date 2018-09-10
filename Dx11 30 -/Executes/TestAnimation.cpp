#include "stdafx.h"
#include "TestAnimation.h"

#include "../Objects/GameModel.h"

TestAnimation::TestAnimation(ExecuteValues * values)
	:Execute(values)
{
	kachujin = new GameModel(
		Models + L"Kachujin/", L"Kachujin.material", 
		Models + L"Kachujin/", L"Kachujin.mesh");
}

TestAnimation::~TestAnimation()
{
	SAFE_DELETE(kachujin);
}

void TestAnimation::Update()
{

}

void TestAnimation::PreRender()
{

}

void TestAnimation::Render()
{

}

void TestAnimation::PostRender()
{

}

