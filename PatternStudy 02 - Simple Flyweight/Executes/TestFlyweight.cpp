#include "stdafx.h"
#include "TestFlyweight.h"

#include "../Renders/Lines/DebugDrawCube.h"


TestFlyweight::TestFlyweight(ExecuteValues * values)
	:Execute(values)

{
	int size = 1000;

	D3DXVECTOR3 center = D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 halfSize = D3DXVECTOR3(0.5f, 0.5f, 0.5f);
	for (int i = 0; i < size; i++) {
		debugs.push_back(new DebugDrawCube(center, halfSize));
		if (i != 0) {
			debugs.back()->Position(
				Math::Random(-200.0f, 200.0f),
				Math::Random(-100.0f, 100.0f),
				Math::Random(-200.0f, 200.0f));
			debugs.back()->RotationDegree(
				Math::Random(-90.0f, 90.0f),
				Math::Random(-90.0f, 90.0f),
				Math::Random(-90.0f, 90.0f));
			debugs.back()->Scale(
				Math::Random(0.5f, 10.0f),
				Math::Random(0.5f, 10.0f),
				Math::Random(0.5f, 10.0f));

			debugs.back()->SetColor(D3DXCOLOR(
				Math::Random(0.0f, 1.0f),
				Math::Random(0.0f, 1.0f),
				Math::Random(0.0f, 1.0f),
				1.0f));			
		}
		//positions.push_back(D3DXVECTOR4(debugs.back()->Position(),1));
		colors.push_back(debugs.back()->GetColor());
		worlds.push_back(debugs.back()->World());
	}

	instance = new DebugDrawCube(center, halfSize);
	//instance->SetPositions(positions);
	instance->SetColors(colors);
	instance->SetWorlds(worlds);
	instance->CreateInstance();

	bInstance = false;
}

TestFlyweight::~TestFlyweight()
{
	for (Line* debug : debugs)
		SAFE_DELETE(debug);

	SAFE_DELETE(instance);
}

void TestFlyweight::Update()
{

}

void TestFlyweight::PreRender()
{

}

void TestFlyweight::Render()
{	
	ImGui::Checkbox("Instance", &bInstance);

	int renderCount = 0;

	if (bInstance == false) {
		for (Line* debug : debugs) {
			debug->Render();
			renderCount++;
		}
	}
	else {
		instance->Render();
		renderCount++;
	}

	ImGui::Text("Render Count : %d", renderCount);

	if (bInstance) {
		ImGui::Begin("Instance");
		instance->PostRender();
		ImGui::End();
	}
}

void TestFlyweight::PostRender()
{

}



