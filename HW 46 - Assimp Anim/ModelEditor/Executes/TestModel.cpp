#include "stdafx.h"
#include "TestModel.h"

#include "Model\Model.h"

#include "assimp\Loader.h"

void TestModel::Initialize()
{
	Loader* loader = NULL;

	loader = new Loader(
		Assets + L"Kachujin/Mesh.fbx", 
		Models + L"Kachujin/", L"Kachujin"
	);
	loader->ExportMaterial();
	loader->ExportMesh();

	SAFE_DELETE(loader);

	//loader = new Loader(
	//	Assets + L"Tank2/tank.fbx", 
	//	Models + L"Tank/", L"Tank"
	//);
	////loader->ExportMaterial();
	//loader->ExportMesh();
	//
	//SAFE_DELETE(loader);

	//loader = new Loader(
	//	Assets + L"Tower/Tower.fbx", 
	//	Models + L"Tower/", L"Tower"
	//);
	//loader->ExportMaterial();
	//loader->ExportMesh();

	//SAFE_DELETE(loader);

	//loader = new Loader(
	//	Assets + L"Chicken/chicken.fbx", 
	//	Models + L"Chicken/", L"Chicken"
	//);
	//loader->ExportMaterial();
	//loader->ExportMesh();

	//SAFE_DELETE(loader);

	//loader = new Loader(
	//	Assets + L"Cottage/cottage.fbx", 
	//	Models + L"Cottage/", L"Cottage"
	//);
	////loader->ExportMaterial();
	//loader->ExportMesh();

	//SAFE_DELETE(loader);

	//loader = new Loader(
	//	Assets + L"Castle/Castle FBX.fbx", 
	//	Models + L"Castle/", L"Castle"
	//);
	////loader->ExportMaterial();
	//loader->ExportMesh();

	//SAFE_DELETE(loader);
}

void TestModel::Ready()
{
	house = NULL;
	//house = new Model();
	//house->ReadMaterial(Models + L"Castle/Castle.material");
	//house->ReadMesh(Models + L"Castle/Castle.mesh");

	houseInstance = NULL;
	//houseInstance = new ModelInstance(house, Shaders + L"068_ModelInstance.fx");

	if (houseInstance != NULL) {
		houseInstance->Ready();
	}

	D3DXMATRIX S, T;
	float scale;

	scale = 0.01f;
	//scale = 0.001f;
	D3DXMatrixScaling(&S, scale, scale, scale);

	D3DXMatrixTranslation(&T, 0, 0, -10);
	D3DXMATRIX world = S * T;
	if (houseInstance != NULL)
		houseInstance->AddWorld(world);

	//D3DXMatrixTranslation(&T, -20, 0, -10);
	//world = S * T;
	//houseInstance->AddWorld(world);

	//D3DXMatrixTranslation(&T, 20, 0, -10);
	//world = S * T;
	//houseInstance->AddWorld(world);

	model = NULL;
	model = new Model();
	model->ReadMaterial(Models + L"Kachujin/Kachujin.material");
	model->ReadMesh(Models + L"Kachujin/Kachujin.mesh");

	//model->ReadMaterial(Models + L"Chicken/Chicken.material");
	//model->ReadMesh(Models + L"Chicken/Chicken.mesh");

	modelInstance = NULL;
	modelInstance = new ModelInstance(model, Shaders + L"068_ModelInstance.fx");

	if (modelInstance != NULL) {
		modelInstance->Ready();
	}

	//scale = 1;
	//D3DXMatrixScaling(&S, scale, scale, scale);
	//D3DXMatrixTranslation(&T, 0,0,0);

	//D3DXMATRIX world = S * T;
	//modelInstance->AddWorld(world);

	//D3DXMatrixTranslation(&T, 5, 0, 0);

	//world = S * T;
	//modelInstance->AddWorld(world);

	for (int i = 0; i < 64; i++) {

		//scale = Math::Random(0.3f, 0.7f);
		scale = Math::Random(0.003f, 0.007f);
		D3DXMatrixScaling(&S, scale, scale, scale);
		D3DXMatrixTranslation(&T, Math::Random(-20.0f, 20.0f), 0, Math::Random(-20.0f, 20.0f));

		D3DXMATRIX world = S * T;
		modelInstance->AddWorld(world);
	}
}

void TestModel::Destroy()
{
	SAFE_DELETE(model);
	SAFE_DELETE(modelInstance);

	SAFE_DELETE(house);
	SAFE_DELETE(houseInstance);
}

void TestModel::Update()
{
	if (houseInstance != NULL)
		houseInstance->Update();

	if (modelInstance != NULL)
		modelInstance->Update();
}

void TestModel::PreRender()
{
}

void TestModel::Render()
{
	if (houseInstance != NULL)
		houseInstance->Render();

	if (modelInstance != NULL)
		modelInstance->Render();
}

void TestModel::PostRender()
{
}

void TestModel::ResizeScreen()
{
}
