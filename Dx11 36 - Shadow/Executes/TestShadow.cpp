#include "stdafx.h"
#include "TestShadow.h"

#include "../Objects/GameAnimModel.h"
#include "../Objects/MeshPlane.h"

#include "../Model/ModelClip.h"

#include "../Lights/Shadow.h"

TestShadow::TestShadow(ExecuteValues * values)
	:Execute(values)
{
	hanjo = new GameAnimModel(
		Models + L"Kachujin/", L"Kachujin.material",
		Models + L"Kachujin/", L"Kachujin.mesh");

	hanjoShader = new Shader(Shaders + L"029_Animation.hlsl");
	hanjo->SetShader(hanjoShader);

	hanjo->SetDiffuse(1, 1, 1, 1);
	hanjo->AddClip(Models + L"Kachujin/Samba_Dance.anim");
	hanjo->Play(0, true, 0, 0.1f);

	hanjo->Scale(0.025f, 0.025f, 0.025f);
	//hanjo->Scale(1.0f, 1.0f, 1.0f);

	plane = new MeshPlane();

	planeShader = new Shader(Shaders + L"033_Plane.hlsl");
	plane->SetShader(planeShader);
	plane->SetDiffuse(1, 1, 1, 1);
	plane->Scale(10, 1, 10);
	plane->SetDiffuseMap(Textures + L"White.png");

	shadow = new Shadow(values);
	shadow->Add(hanjo);
	shadow->Add(plane);


}

TestShadow::~TestShadow()
{
	SAFE_DELETE(shadow);

	SAFE_DELETE(hanjo);
	SAFE_DELETE(hanjoShader);

	SAFE_DELETE(plane);
	SAFE_DELETE(planeShader);
}

void TestShadow::Update()
{
	shadow->Update();

	//plane->Update();
	//hanjo->Update();
}

void TestShadow::PreRender()
{
	shadow->PreRender();
}

void TestShadow::Render()
{
	//plane->Render();
	//hanjo->Render();

	shadow->Render();
}

void TestShadow::PostRender()
{

}

