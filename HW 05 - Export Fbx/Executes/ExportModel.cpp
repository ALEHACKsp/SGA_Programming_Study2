#include "stdafx.h"
#include "ExportModel.h"

#include "../Fbx/Exporter.h"

#include "../Objects/GameModel.h"

#define MESH_SIZE 7

ExportModel::ExportModel(ExecuteValues * values)
	:Execute(values)
{
	Fbx::Exporter* exporter = { NULL };

	// Cube
	//exporter = new Fbx::Exporter(Assets + L"Meshes/Cube.fbx");
	//exporter->ExportMaterial(Models + L"Meshes/Cube/", L"Cube.material");

	wstring path = L"Meshes/";
	wstring meshes[] = {
		L"Capsule",
		L"Cube",
		L"Cylinder",
		L"Plane",
		L"Quad",
		L"Sphere",
		L"Teapot" };

	for (int i = 0; i < MESH_SIZE; i++) {
		exporter = new Fbx::Exporter(Assets + path + meshes[i] + L".fbx");
		exporter->ExportMaterial(Models + path + meshes[i] + L"/",
			meshes[i] + L".material");
		exporter->ExportMesh(Models + path + meshes[i] + L"/",
			meshes[i] + L".mesh");

		SAFE_DELETE(exporter);
	}

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

