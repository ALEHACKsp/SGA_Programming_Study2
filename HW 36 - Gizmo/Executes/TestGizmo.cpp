#include "stdafx.h"
#include "TestGizmo.h"

#include "../Draw/MeshCube.h"
#include "../Draw/MeshSphere.h"
#include "../Draw/MeshGrid.h"

#include "./Renders/Gizmo/GizmoModel.h"
#include "./Renders/Gizmo/Gizmo.h"

TestGizmo::TestGizmo(ExecuteValues * values)
	:Execute(values)
{
	gridMaterial = new Material(Shaders + L"Homework/LineGrid.hlsl");
	////gridMaterial->SetDiffuseMap(Textures + L"White.png");
	////grid = new MeshGrid(gridMaterial, 4, 4, 16, 16, true);
	grid = new MeshGrid(gridMaterial, 128, 128, 128, 128, true);
	////grid = new MeshGrid(gridMaterial, 256, 256, 128, 128, true);

	ray = new Ray();

	cubeMaterial = new Material(Shaders + L"Homework/Mesh3.hlsl");
	cubeMaterial->SetDiffuseMap(Textures + L"White.png");
	////cubeMaterial->SetDiffuseMap(Textures + L"Red.png");
	cube = new MeshCube(cubeMaterial, 1, 1, 1);
	cube->Position(0.5f, 0.5f, 0.5f);

	cubeBounding = new BBox();
	cubeBounding->Min = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
	cubeBounding->Max = D3DXVECTOR3(+0.5f, +0.5f, +0.5f);

	cubeLine = new DebugLine();

	//rotate = new GizmoModel(GizmoModel::Kind::Rotate);
	//translate = new GizmoModel(GizmoModel::Kind::Translate);
	//scale = new GizmoModel(GizmoModel::Kind::Scale);
	////rotate = translate = scale = NULL;
	//
	//rotate->Position(0, 10, 30);
	//rotate->SetColor(D3DXCOLOR(0, 0, 1, 1));
	//translate->Position(0, 10, 10);
	//translate->SetColor(D3DXCOLOR(1, 0, 0, 1));
	//scale->Position(0, 10, 30);
	//scale->SetColor(D3DXCOLOR(0, 1, 0, 1));
	
	gizmo = new Gizmo(values);
}

TestGizmo::~TestGizmo()
{
	//SAFE_DELETE(scale);
	//SAFE_DELETE(rotate);
	//SAFE_DELETE(translate);

	SAFE_DELETE(grid);
	SAFE_DELETE(gridMaterial);

	SAFE_DELETE(ray);

	SAFE_DELETE(cube);
	SAFE_DELETE(cubeMaterial);
	SAFE_DELETE(cubeBounding);
	SAFE_DELETE(cubeLine);

	SAFE_DELETE(gizmo);
}

void TestGizmo::Update()
{
	D3DXMATRIX V, P;
	values->MainCamera->Matrix(&V);
	values->Perspective->GetMatrix(&P);

	D3DXVECTOR3 S;
	values->MainCamera->Position(&S);

	D3DXMATRIX W = cube->World();
	values->Viewport->GetRay(ray, S, W, V, P);

	float result;
	if (Mouse::Get()->Down(0)) {
		bPicking = cubeBounding->Intersect(ray, result);
		if (bPicking) {
			gizmo->Visible(true);

			gizmo->Set(cube);

		}
	}

	if (bPicking) {
		gizmo->Update(ray);
	}
}

void TestGizmo::PreRender()
{
	
}

void TestGizmo::Render()
{
	ImGui::Checkbox("Cube Picking", &bPicking);

	D3DXVECTOR3 pos = cube->Position();
	if (ImGui::DragFloat3("Cube Position", (float*)&pos, 0.1f)) {
		cube->Position(pos);
		D3DXMATRIX W = cube->World();
		cubeLine->Draw(W, cubeBounding);
	}

	grid->Render();

	cubeLine->Render();
	cube->Render();

	//rotate->Render();
	//scale->Render();
	//translate->Render();
	
	gizmo->Render();
}

void TestGizmo::PostRender()
{
}

