#include "stdafx.h"
#include "TestRenderTarget.h"

#include "../LandScape/Sky.h"

#include "../Objects/MeshPlane.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/MeshCube.h"

#include "../PostEffects/GaussianBlur.h"

#include "../Objects/PointLight.h"

TestRenderTarget::TestRenderTarget(ExecuteValues * values)
	:Execute(values)
{
	sky = new Sky(values);

	shader = new Shader(Shaders + L"027_NormalMap.hlsl");

	renderTarget[0] = new RenderTarget(values);
	renderTarget[1] = new RenderTarget(values);
	renderTarget[2] = new RenderTarget(values);

	pointLight = new PointLight();

	{
		PointLight::Desc desc;
		desc.Color = D3DXVECTOR3(1, 0, 0);
		desc.Intensity = 3;
		desc.Position = D3DXVECTOR3(3, 0, 0);
		desc.Range = 5;

		pointLight->Add(desc);
	}


	// Create Render2D
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		postEffect[0] = new GaussianBlur(values, "PS_X");
		postEffect[0]->Scale(desc.Width, desc.Height);

		postEffect[1] = new GaussianBlur(values, "PS_Y");
		postEffect[1]->Scale(desc.Width, desc.Height);

		postEffect[2] = new GaussianBlur(values, "PS_XY");
		postEffect[2]->Scale(desc.Width, desc.Height);
	}

	plane = new MeshPlane();
	plane->Scale(3, 3, 3);
	plane->SetShader(shader);
	plane->SetDiffuse(1, 1, 1, 1);
	plane->SetDiffuseMap(Textures + L"Floor.png");
	plane->SetNormalMap(Textures + L"Floor_normal.png");

	sphere = new MeshSphere();
	sphere->Scale(3, 3, 3);
	sphere->Position(7.0f, 1.5f, 0.0f);
	sphere->SetShader(shader);
	sphere->SetDiffuse(1, 1, 1, 1);
	sphere->SetDiffuseMap(Textures + L"Wall.png");
	sphere->SetNormalMap(Textures + L"Wall_normal.png");

	cube[0] = new MeshCube();
	cube[0]->Scale(2, 2, 2);
	cube[0]->Position(-5.0f, 1.0f, 0.0f);
	cube[0]->SetShader(shader);
	cube[0]->SetDiffuse(1, 1, 1, 1);
	cube[0]->SetDiffuseMap(Textures + L"Bricks.png");
	cube[0]->SetNormalMap(Textures + L"Bricks_normal.png");

	cube[1] = new MeshCube();
	cube[1]->Scale(2, 2, 2);
	cube[1]->Position(0.0f, 1.0f, 0.0f);
	cube[1]->SetShader(shader);
	cube[1]->SetDiffuse(1, 1, 1, 1);
	cube[1]->SetDiffuseMap(Textures + L"Stones.png");
	cube[1]->SetNormalMap(Textures + L"Stones_normal.png");
}

TestRenderTarget::~TestRenderTarget()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(sphere);
	SAFE_DELETE(plane);
	SAFE_DELETE(cube[0]);
	SAFE_DELETE(cube[1]);

	SAFE_DELETE(sky);

	SAFE_DELETE(renderTarget[0]);
	SAFE_DELETE(renderTarget[1]);
	SAFE_DELETE(renderTarget[2]);
	SAFE_DELETE(postEffect[0]);
	SAFE_DELETE(postEffect[1]);
	SAFE_DELETE(postEffect[2]);

	SAFE_DELETE(pointLight);
}

void TestRenderTarget::Update()
{
	sky->Update();

	pointLight->Update();

	plane->Update();
	sphere->Update();
	cube[0]->Update();
	cube[1]->Update();
}

void TestRenderTarget::PreRender()
{
	renderTarget[0]->Set();

	sky->Render();

	plane->Render();
	sphere->Render();
	cube[0]->Render();
	cube[1]->Render();

	// X
	renderTarget[1]->Set();

	postEffect[0]->Update();
	postEffect[0]->SRV(renderTarget[0]->GetSRV());
	postEffect[0]->Render();

	// Y
	renderTarget[2]->Set();

	postEffect[1]->Update();
	postEffect[1]->SRV(renderTarget[1]->GetSRV());
	postEffect[1]->Render();
}

void TestRenderTarget::Render()
{

}

void TestRenderTarget::PostRender()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	postEffect[2]->Position(0, 0);
	postEffect[2]->Scale(desc.Width, desc.Height);
	postEffect[2]->Update();
	postEffect[2]->SRV(renderTarget[2]->GetSRV());
	postEffect[2]->Render();

	postEffect[2]->Position(0, 0);
	postEffect[2]->Scale(300, 300);
	postEffect[2]->Update();
	postEffect[2]->SRV(renderTarget[0]->GetSRV());
	postEffect[2]->Render();

	postEffect[2]->Position(300, 0);
	postEffect[2]->Scale(300, 300);
	postEffect[2]->Update();
	postEffect[2]->SRV(renderTarget[1]->GetSRV());
	postEffect[2]->Render();

	postEffect[2]->Position(600, 0);
	postEffect[2]->Scale(300, 300);
	postEffect[2]->Update();
	postEffect[2]->SRV(renderTarget[2]->GetSRV());
	postEffect[2]->Render();
}

