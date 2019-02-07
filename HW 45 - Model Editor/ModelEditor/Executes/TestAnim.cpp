#include "stdafx.h"
#include "TestAnim.h"

#include "Model\Model.h"
#include "Model\ModelClip.h"

#include "Fbx\FbxLoader.h"

void TestAnim::Initialize()
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

	//loader = new FbxLoader(
	//	Assets + L"Kachujin/Running.fbx",
	//	Models + L"Kachujin/", L"Running"
	//);
	//loader->ExportAnimation(0);
	//SAFE_DELETE(loader);

	//loader = new FbxLoader(
	//	Assets + L"Kachujin/Taunt.fbx",
	//	Models + L"Kachujin/", L"Taunt"
	//);
	//loader->ExportAnimation(0);
	//SAFE_DELETE(loader);

	SAFE_DELETE(loader);
}

void TestAnim::Ready()
{
	model = NULL;
	model = new Model();
	model->ReadMaterial(Models + L"Kachujin/Kachujin.material");
	model->ReadMesh(Models + L"Kachujin/Kachujin.mesh");

	clips.push_back(new ModelClip(Models + L"Kachujin/Samba_Dance.animation"));
	clipNames.push_back("Samba_Dance");
	clips.push_back(new ModelClip(Models + L"Kachujin/Running.animation"));
	clipNames.push_back("Running");
	clips.push_back(new ModelClip(Models + L"Kachujin/Taunt.animation"));
	clipNames.push_back("Taunt");

	animInstance = NULL;
	animInstance = new AnimInstance(model, clips, Shaders + L"Homework/AnimInstance.fx", clipNames);
	//animInstance = new AnimInstance(model, clip, Shaders + L"069_AnimInstance.fx");
	//animInstance = new AnimInstance(model, clip, Shaders + L"070_ClipDistance.fx");

	if (animInstance != NULL) {
		animInstance->Ready();
	}

	D3DXMATRIX S, T;
	float scale;

	scale = 1.0f;
	D3DXMatrixScaling(&S, scale, scale, scale);
	D3DXMatrixTranslation(&T, 0,0,0);

	D3DXMATRIX world = S * T;
	//animInstance->AddWorld(world);

	D3DXMatrixTranslation(&T, 5, 0, 0);

	world = S * T;
	//animInstance->AddWorld(world);

	for (int i = -5; i <= 5; i++) 
	{
		D3DXMATRIX S, T;
		float scale;

		for (int z = -2; z <= 2; z++) 
		{
			scale = 0.5f;
			D3DXMatrixScaling(&S, scale, scale, scale);
			D3DXMatrixTranslation(&T, i * 2.5f, 0, z * 2);

			D3DXMATRIX world = S * T;
			animInstance->AddWorld(world);
		}
	}
}

void TestAnim::Destroy()
{
	SAFE_DELETE(model);
	SAFE_DELETE(animInstance);
	for (ModelClip* clip : clips)
		SAFE_DELETE(clip);
}

void TestAnim::Update()
{
	if (animInstance != NULL)
		animInstance->Update();

	if (Keyboard::Get()->Down(VK_SPACE)) {
		D3DXMATRIX S, T;
		float scale;


		scale = Math::Random(0.3f, 0.7f);
		D3DXMatrixScaling(&S, scale, scale, scale);
		D3DXMatrixTranslation(&T, Math::Random(-20.0f, 20.0f), 0, Math::Random(-20.0f, 20.0f));

		D3DXMATRIX world = S * T;
		animInstance->AddWorld(world);
	}

}

void TestAnim::PreRender()
{
}

void TestAnim::Render()
{
	//ImGui::Begin("Clip Plane");

	//static D3DXPLANE plane = D3DXPLANE(-1, 0, 0, 0);

	//for (Material* material : model->Materials())
	//	material->GetShader()->AsVector("ClipPlane")->SetFloatVector(plane);

	//ImGui::End();

	if (animInstance != NULL)
		animInstance->Render();
}

void TestAnim::PostRender()
{
}

void TestAnim::ResizeScreen()
{
}
