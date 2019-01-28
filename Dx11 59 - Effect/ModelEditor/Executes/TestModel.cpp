#include "stdafx.h"
#include "TestModel.h"

#include "Fbx\FbxLoader.h"

void TestModel::Initialize()
{
	FbxLoader* loader = NULL;

	//loader = new FbxLoader(
	//	Assets + L"Kachujin/Mesh.fbx", 
	//	Models + L"Kachujin/", L"Kachujin"
	//);
	//loader->ExportMaterial();
	//loader->ExportMesh();

	//SAFE_DELETE(loader);

	//loader = new FbxLoader(
	//	Assets + L"Tank/tank.fbx", 
	//	Models + L"Tank/", L"Tank"
	//);
	//loader->ExportMaterial();
	//loader->ExportMesh();

	SAFE_DELETE(loader);
}

void TestModel::Ready()
{
	model = NULL;
	model = new GameModel(
		Shaders + L"064_Model.fx",
		Models + L"Tank/Tank.material",
		Models + L"Tank/Tank.mesh"
	);

	if (model != NULL) {
		model->Ready();

		//model->Position(0, 0, 0);

		model->SetDiffuse(0.5f, 0.5f, 0.5f);
		model->SetSpecular(0.8f, 0.8f, 0.8f, 16.0f);
	}
}

void TestModel::Destroy()
{
	SAFE_DELETE(model);
}

void TestModel::Update()
{
	if (model != NULL)
		model->Update();
}

void TestModel::PreRender()
{
}

void TestModel::Render()
{
	if (model != NULL)
		model->Render();
}

void TestModel::PostRender()
{
}

void TestModel::ResizeScreen()
{
}
