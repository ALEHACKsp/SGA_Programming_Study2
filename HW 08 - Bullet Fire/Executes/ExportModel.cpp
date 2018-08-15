#include "stdafx.h"
#include "ExportModel.h"

#include "../Fbx/Exporter.h"

#include "../Objects/GameModel.h"

ExportModel::ExportModel(ExecuteValues * values)
	:Execute(values)
{
	Fbx::Exporter* exporter = NULL;

	// Cube
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cube.fbx");
	//exporter->ExportMaterial(Models + L"/Meshes/Cube/", L"Cube.material");
	//exporter->ExportMesh(Models + L"/Meshes/Cube/", L"Cube.mesh");

	int a = 10;

	// Tank
	//exporter = new Fbx::Exporter(Assets + L"Tank/Tank.fbx");
	//exporter->ExportMaterial(Models + L"/Tank/", L"Tank.material");
	//exporter->ExportMesh(Models + L"/Tank/", L"Tank.mesh");
	//SAFE_DELETE(exporter);

	// GameModel �ȿ��� ����� �����ߵ�
	//Model* model = new Model();
	//model->ReadMaterial(Models + L"/Tank/", L"Tank.material");
	//model->ReadMesh(Models + L"/Tank/", L"Tank.mesh");

	tank = new GameModel(
		Models + L"/Tank/", L"Tank.material",
		Models + L"/Tank/", L"Tank.mesh"
	);

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

