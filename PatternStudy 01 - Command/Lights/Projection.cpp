#include "stdafx.h"
#include "Projection.h"

#include "../Viewer/Fixity.h"
#include "../Viewer/Perspective.h"
#include "../Objects/GameModel.h"

Projection::Projection(ExecuteValues * values)
	:values(values)
{
	shader = new Shader(Shaders + L"034_Projection.hlsl");

	fixity = new Fixity();
	fixity->Position(0, 10, 0);
	fixity->RotationDegree(90, 0);

	// Perspective
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		// width height 너비 높이에 대한 비율임
		// pi * 0.5f 로 준다는 화면각 그대로 쓴다는거 1,1 비율까지 이렇게하면 화면 그대로 쓴다는거
		// 정투영
		perspective = new Perspective(1, 1, Math::PI * 0.5f, 1, 100);
	}

	vpBuffer = new ViewProjectionBuffer();
}

Projection::~Projection()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(fixity);
	SAFE_DELETE(perspective);

	SAFE_DELETE(vpBuffer);
}

void Projection::Add(GameModel * obj)
{
	objs.push_back(obj);
}

void Projection::Update()
{
	for (GameModel* obj : objs)
	{
		obj->SetShader(shader);
		obj->SetNormalMap(Textures + L"Bricks.png");
		obj->Update();
	}

	D3DXMATRIX V, P;
	fixity->Matrix(&V);
	vpBuffer->SetView(V);

	perspective->GetMatrix(&P);
	vpBuffer->SetProjection(P);
}

void Projection::PreRender()
{
}

void Projection::Render()
{
	D3DXVECTOR3 position;
	fixity->Position(&position);
	ImGui::SliderFloat3("Proj Position", (float *)&position,-100, 100);
	fixity->Position(position.x, position.y, position.z);

	D3DXVECTOR2 rotation;
	fixity->RotationDegree(&rotation);
	ImGui::SliderFloat2("Proj Rotation", (float *)&rotation, -180, 180);
	fixity->RotationDegree(rotation.x, rotation.y);

	vpBuffer->SetVSBuffer(10);

	for (GameModel* obj : objs)
	{
		obj->Render();
	}
}
