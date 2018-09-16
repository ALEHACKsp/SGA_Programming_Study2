#include "stdafx.h"
#include "TestAnimation.h"

#include "../Objects/GameAnimModel.h"

TestAnimation::TestAnimation(ExecuteValues * values)
	:Execute(values)
{
	kachujin = new GameAnimModel(
		Models + L"Kachujin/", L"Kachujin.material", 
		Models + L"Kachujin/", L"Kachujin.mesh");
	shader = new Shader(Shaders + L"/Homework/Animation.hlsl");
	kachujin->SetShader(shader);
	//kachujin->SetDiffuseMap(Models + L"Kachujin/Kachujin_diffuse.png");
	kachujin->SetNormalMap(Models + L"Kachujin/Kachujin_normal.png");
	kachujin->SetSpecularMap(Models + L"Kachujin/Kachujin_specular.png");

	kachujin->AddClip(Models + L"Kachujin/Samba_Dance.anim");
	kachujin->LockRoot(0, false);
	kachujin->Repeat(0, true);
	kachujin->Speed(0, 15.0f);
}

TestAnimation::~TestAnimation()
{
	SAFE_DELETE(kachujin);
	SAFE_DELETE(shader);
}

void TestAnimation::Update()
{
	kachujin->Update();
}

void TestAnimation::PreRender()
{

}

void TestAnimation::Render()
{
	kachujin->Render();
}

void TestAnimation::PostRender()
{
	//kachujin->Scale(D3DXVECTOR3(0.5f, 0.5f, 0.5f));
	
	//ModelBone* bone = kachujin->GetModel()->BoneByIndex(0);

	//D3DXMATRIX local = bone->Local();
	//local._11 = 0.5f;
	//local._22 = 0.5f;
	//local._33 = 0.5f;
	//bone->Local(local);

	//for (int i = 0; i < kachujin->GetModel()->BoneCount(); i++) {
	//	ModelBone* bone = kachujin->GetModel()->BoneByIndex(i);

	//	//D3DXMATRIX local = bone->Local();
	//	//local._11 = 0.5f;
	//	//local._22 = 0.5f;
	//	//local._33 = 0.5f;
	//	//bone->Local(local);

	//	//D3DXMATRIX global = bone->Global();
	//	//global._11 = 0.5f;
	//	//global._22 = 0.5f;
	//	//global._33 = 0.5f;
	//	//bone->Global(global);
	//}
}

