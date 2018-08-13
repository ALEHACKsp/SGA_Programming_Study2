#include "stdafx.h"
#include "ExportModel.h"

#include "../Fbx/Exporter.h"

#include "../Objects/GameTank.h"

ExportModel::ExportModel(ExecuteValues * values)
	:Execute(values)
{
	Fbx::Exporter* exporter = NULL;

	// Cube
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cube.fbx");
	//exporter->ExportMaterial(Models + L"/Meshes/Cube/", L"Cube.material");

	// Tank
	//exporter = new Fbx::Exporter(Assets + L"Tank/Tank.fbx");
	//exporter->ExportMaterial(Models + L"/Tank/", L"Tank.material");
	//exporter->ExportMesh(Models + L"/Tank/", L"Tank.mesh");
	//SAFE_DELETE(exporter);

	// GameModel 안에서 만드니 지워야됨
	//Model* model = new Model();
	//model->ReadMaterial(Models + L"/Tank/", L"Tank.material");
	//model->ReadMesh(Models + L"/Tank/", L"Tank.mesh");

	tank = new GameTank(Models + L"/Tank/", L"Tank.material");

	D3DXMATRIX R;
	D3DXMatrixRotationY(&R, Math::ToRadian(90));

	tank->RootAxis(R);

	int a = 10;
}

ExportModel::~ExportModel()
{
	SAFE_DELETE(tank);
}

void ExportModel::Update()
{
	tank->Update();
}

void ExportModel::PreRender()
{
	
}

void ExportModel::Render()
{
	tank->Render();
}

void ExportModel::PostRender()
{
	
}

void ExportModel::ResizeScreen()
{
}

