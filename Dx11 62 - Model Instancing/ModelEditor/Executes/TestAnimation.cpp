#include "stdafx.h"
#include "TestAnimation.h"

#include "Fbx\FbxLoader.h"

void TestAnimation::Initialize()
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
	//SAFE_DELETE(loader);

	model = NULL;
	//model = new GameModel(
	//	Shaders + L"064_Model.fx",
	//	Models + L"Kachujin/Kachujin.material",
	//	Models + L"Kachujin/Kachujin.mesh"
	//);

	animator = NULL;

	animator = new GameAnimator(
		Shaders + L"064_Model.fx",
		Models + L"Kachujin/Kachujin.material",
		Models + L"Kachujin/Kachujin.mesh"
	);
}

void TestAnimation::Ready()
{

	if (model != NULL)
		model->Ready();

	if (animator != NULL) {
		animator->Ready();

		animator->AddClip(Models + L"Kachujin/Samba_Dance.animation");

		animator->Position(0, 0, 0);

		animator->SetDiffuse(0.8f, 0.8f, 0.8f);
		//animator->SetSpecular(0.8f, 0.8f, 0.8f, 16.0f);
		animator->SetSpecular(1.0f, 1.0f, 1.0f, 16.0f);
	}
}

void TestAnimation::Destroy()
{
	SAFE_DELETE(model);
	SAFE_DELETE(animator);
}

void TestAnimation::Update()
{
	if (model != NULL)
		model->Update();

	if (animator != NULL)
		animator->Update();
}

void TestAnimation::PreRender()
{
}

void TestAnimation::Render()
{
	if (model != NULL)
		model->Render();

	if (animator != NULL)
		animator->Render();
}

void TestAnimation::PostRender()
{
}

void TestAnimation::ResizeScreen()
{
}
