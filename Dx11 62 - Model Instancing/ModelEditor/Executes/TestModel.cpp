#include "stdafx.h"
#include "TestModel.h"

#include "Model\Model.h"

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
	model = new Model();
	model->ReadMaterial(Models + L"Tank/Tank.material");
	model->ReadMesh(Models + L"Tank/Tank.mesh");

	modelInstance = NULL;
	modelInstance = new ModelInstance(model, Shaders + L"068_ModelInstance.fx");

	if (modelInstance != NULL) {
		modelInstance->Ready();
	}

	D3DXMATRIX S, T;
	float scale;

	scale = 1;
	D3DXMatrixScaling(&S, scale, scale, scale);
	D3DXMatrixTranslation(&T, 0,0,0);

	D3DXMATRIX world = S * T;
	modelInstance->AddWorld(world);

	D3DXMatrixTranslation(&T, 5, 0, 0);

	world = S * T;
	modelInstance->AddWorld(world);

	//for (int i = 0; i < 64; i++) {

	//	scale = Math::Random(0.3f, 0.7f);
	//	D3DXMatrixScaling(&S, scale, scale, scale);
	//	D3DXMatrixTranslation(&T, Math::Random(-20.0f, 20.0f), 0, Math::Random(-20.0f, 20.0f));

	//	D3DXMATRIX world = S * T;
	//	modelInstance->AddWorld(world);
	//}
}

void TestModel::Destroy()
{
	SAFE_DELETE(model);
	SAFE_DELETE(modelInstance);
}

void TestModel::Update()
{
	if (modelInstance != NULL)
		modelInstance->Update();
}

void TestModel::PreRender()
{
}

void TestModel::Render()
{
	if (modelInstance != NULL)
		modelInstance->Render();
}

void TestModel::PostRender()
{
}

void TestModel::ResizeScreen()
{
}
