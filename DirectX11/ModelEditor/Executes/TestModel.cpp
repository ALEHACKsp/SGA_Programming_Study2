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
	//	Assets + L"Kachujin/Samba_Dancing.fbx",
	//	Models + L"Kachujin/", L"Samba_Dance"
	//);
	//loader->ExportAnimation(0);
	SAFE_DELETE(loader);
}

void TestModel::Ready()
{
	//models.push_back(new GameModel(
	//	Shaders + L"052_Model.fx",
	//	Models + L"Kachujin/Kachujin.material",
	//	Models + L"Kachujin/Kachujin.mesh"
	//));

	animator = new GameAnimator(
		Shaders + L"052_Model.fx",
		Models + L"Kachujin/Kachujin.material",
		Models + L"Kachujin/Kachujin.mesh"
	);
	animator->AddClip(Models + L"Kachujin/Samba_Dance.animation");
}

void TestModel::Destroy()
{
	for (GameModel* model : models)
		SAFE_DELETE(model);
	
	SAFE_DELETE(animator);
}

void TestModel::Update()
{
	for (GameModel* model : models)
		model->Update();

	animator->Update();
}

void TestModel::PreRender()
{
}

void TestModel::Render()
{
	for (GameModel* model : models)
		model->Render();

	animator->Render();
}

void TestModel::PostRender()
{
}

void TestModel::ResizeScreen()
{
}
