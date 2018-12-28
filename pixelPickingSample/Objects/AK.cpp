#include "stdafx.h"
#include "AK.h"
#include "Physics\RayCollider.h"

AK::AK()
	: GameModel(Materials + L"AK/", L"AK.material", Models + L"AK/", L"AK.mesh")
{
	SetShader(Shaders + L"200_PBR.hlsl");

	collider = new RayCollider(D3DXVECTOR3(0, 25, -180), D3DXVECTOR3(0, 0, -1), this->GetTransform());
}

AK::~AK()
{
}

void AK::Update()
{
	__super::Update();
}

void AK::Render()
{
	__super::Render();

	//ImGui::Begin("AK");
	//{
	//	D3DXVECTOR3 vec;
	//	vec = Scale();
	//	ImGui::SliderFloat3("Scale", (float*)&vec, -50, 50);
	//	Scale(vec);
	//	vec = Position();
	//	ImGui::SliderFloat3("Position", (float*)&vec, -50, 50);
	//	Position(vec);
	//	vec = RotationDegree();
	//	ImGui::SliderFloat3("Rotation", (float*)&vec, -180, 180);
	//	RotationDegree(vec);
	//}
	//ImGui::End();
}
