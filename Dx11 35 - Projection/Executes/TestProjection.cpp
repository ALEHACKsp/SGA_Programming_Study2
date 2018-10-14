#include "stdafx.h"
#include "TestProjection.h"

#include "../Objects/GameAnimModel.h"
#include "../Objects/MeshPlane.h"

#include "../Model/ModelClip.h"

#include "../Lights/Projection.h"

TestProjection::TestProjection(ExecuteValues * values)
	:Execute(values)
{
	hanjo = new GameAnimModel(
		Models + L"Kachujin/", L"Kachujin.material",
		Models + L"Kachujin/", L"Kachujin.mesh");

	hanjoShader = new Shader(Shaders + L"029_Animation.hlsl");
	hanjo->SetShader(hanjoShader);

	hanjo->AddClip(Models + L"Kachujin/Samba_Dance.anim");
	hanjo->Play(0, true, 0, 10.0f);

	hanjo->Scale(0.025f, 0.025f, 0.025f);

	plane = new MeshPlane();

	planeShader = new Shader(Shaders + L"033_Plane.hlsl");
	plane->SetShader(planeShader);
	plane->SetDiffuse(1, 1, 1, 1);
	plane->Scale(10, 1, 10);

	projection = new Projection(values);
	//projection->Add(hanjo);
	projection->Add(plane);
}

TestProjection::~TestProjection()
{
	SAFE_DELETE(projection);

	SAFE_DELETE(hanjo);
	SAFE_DELETE(hanjoShader);

	SAFE_DELETE(plane);
	SAFE_DELETE(planeShader);
}

void TestProjection::Update()
{
	projection->Update();

	//plane->Update();
	hanjo->Update();
}

void TestProjection::PreRender()
{

}

void TestProjection::Render()
{
	//plane->Render();
	hanjo->Render();

	projection->Render();
}

void TestProjection::PostRender()
{

}

