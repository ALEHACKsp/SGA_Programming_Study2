#include "stdafx.h"
#include "ExportModel.h"

#include "../Fbx/Exporter.h"

ExportModel::ExportModel(ExecuteValues * values)
	:Execute(values)
{
	Fbx::Exporter* exporter = NULL;

	//// Cube
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cube.fbx");
	//exporter->ExportMaterial(Models + L"/Meshes/Cube/", L"Cube.material");

	//// Tank
	//exporter = new Fbx::Exporter(Assets + L"Tank/Tank.fbx");
	//exporter->ExportMaterial(Models + L"/Tank/", L"Tank.material");
	//exporter->ExportMesh(Models + L"/Tank/", L"Tank.mesh");
	//SAFE_DELETE(exporter);

	// GameModel 안에서 만드니 지워야됨
	//Model* model = new Model();
	//model->ReadMaterial(Models + L"/Tank/", L"Tank.material");
	//model->ReadMesh(Models + L"/Tank/", L"Tank.mesh");

}

ExportModel::~ExportModel()
{

}

void ExportModel::Update()
{

}

void ExportModel::PreRender()
{
	
}

void ExportModel::Render()
{

}

void ExportModel::PostRender()
{
	
}

void ExportModel::ResizeScreen()
{
}

