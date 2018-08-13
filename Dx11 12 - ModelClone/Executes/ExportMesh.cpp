#include "stdafx.h"
#include "ExportMesh.h"

#include "../Fbx/Exporter.h"

#include "../Objects/GameModel.h"

ExportMesh::ExportMesh(ExecuteValues * values)
	:Execute(values)
{
	//Fbx::Exporter* exporter = NULL;

	//// Capsule
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Capsule.fbx");
	//exporter->ExportMaterial(Materials + L"/Meshes/", L"Capsule.material");
	//exporter->ExportMesh(Models + L"/Meshes/", L"Capsule.mesh");
	//SAFE_DELETE(exporter);

	//// Cube
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cube.fbx");
	//exporter->ExportMaterial(Materials + L"/Meshes/", L"Cube.material");
	//exporter->ExportMesh(Models + L"/Meshes/", L"Cube.mesh");
	//SAFE_DELETE(exporter);

	//// Cylinder
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cylinder.fbx");
	//exporter->ExportMaterial(Materials + L"/Meshes/", L"Cylinder.material");
	//exporter->ExportMesh(Models + L"/Meshes/", L"Cylinder.mesh");
	//SAFE_DELETE(exporter);

	//// Plane
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Plane.fbx");
	//exporter->ExportMaterial(Materials + L"/Meshes/", L"Plane.material");
	//exporter->ExportMesh(Models + L"/Meshes/", L"Plane.mesh");
	//SAFE_DELETE(exporter);

	//// Quad
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Quad.fbx");
	//exporter->ExportMaterial(Materials + L"/Meshes/", L"Quad.material");
	//exporter->ExportMesh(Models + L"/Meshes/", L"Quad.mesh");
	//SAFE_DELETE(exporter);

	//// Sphere
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Sphere.fbx");
	//exporter->ExportMaterial(Materials + L"/Meshes/", L"Sphere.material");
	//exporter->ExportMesh(Models + L"/Meshes/", L"Sphere.mesh");
	//SAFE_DELETE(exporter);

	//// Teapot
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Teapot.fbx");
	//exporter->ExportMaterial(Materials + L"/Meshes/", L"Teapot.material");
	//exporter->ExportMesh(Models + L"/Meshes/", L"Teapot.mesh");
	//SAFE_DELETE(exporter);

	// running 타임 게임 시작되서 진행된 시간
	//float start = Time::Get()->Running();
	for (int i = 0; i < 1; i++) {
		GameModel * model = new GameModel
		{
			Materials + L"/Meshes/", L"Teapot.material",
			Models + L"/Meshes/", L"Teapot.mesh"
		};

		D3DXVECTOR3 temp;
		temp.x = Math::Random(-20, 20);
		temp.y = 0;
		temp.z = Math::Random(-20, 20);
		model->Position(temp);

		D3DXMATRIX R;
		D3DXMatrixRotationY(&R, Math::ToRadian(Math::Random(-90, 90)));
		model->RootAxis(R);

		models.push_back(model);
	}
	float end = Time::Get()->Running();

	// Update 들어가야 시간 지나감 나중에 다시 해보자
	//wstring str = L"Execute Time : " + to_wstring(end - start);
	//D3DDesc desc;
	//D3D::GetDesc(&desc);
	//// NULL 들어가면 부모의 묶임 enter쳐야됨? handle 넣으면 그냥 넘어감
	// 시간나면 조사해보자
	//MessageBox(desc.Handle, str.c_str(), L"", MB_OK);
}

ExportMesh::~ExportMesh()
{
	for (GameModel * model : models)
		SAFE_DELETE(model);
}

void ExportMesh::Update()
{
	for (GameModel * model : models)
		model->Update();
}

void ExportMesh::PreRender()
{
	
}

void ExportMesh::Render()
{
	for (GameModel * model : models)
		model->Render();
}

void ExportMesh::PostRender()
{
	
}

void ExportMesh::ResizeScreen()
{
}

