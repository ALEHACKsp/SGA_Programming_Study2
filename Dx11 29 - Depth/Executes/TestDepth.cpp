#include "stdafx.h"
#include "TestDepth.h"

#include "../LandScape/Sky.h"

#include "../Objects/MeshPlane.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/MeshCube.h"

TestDepth::TestDepth(ExecuteValues * values)
	:Execute(values)
{
	sky = new Sky(values);

	shader = new Shader(Shaders + L"027_NormalMap.hlsl");
	depthShader = new Shader(Shaders + L"028_Depth.hlsl");

	renderTarget[0] = new RenderTarget(values);
	renderTarget[1] = new RenderTarget(values);

	postEffect = new Render2D(values);

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

TestDepth::~TestDepth()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(depthShader);

	SAFE_DELETE(sphere);
	SAFE_DELETE(plane);
	SAFE_DELETE(cube[0]);
	SAFE_DELETE(cube[1]);

	SAFE_DELETE(sky);

	SAFE_DELETE(renderTarget[0]);
	SAFE_DELETE(renderTarget[1]);
	SAFE_DELETE(postEffect);

}

void TestDepth::Update()
{
	sky->Update();

	plane->Update();
	sphere->Update();
	cube[0]->Update();
	cube[1]->Update();
}

void TestDepth::PreRender()
{
	renderTarget[0]->Set();

	plane->SetShader(shader);
	sphere->SetShader(shader);
	cube[0]->SetShader(shader);
	cube[1]->SetShader(shader);

	plane->Render();
	sphere->Render();
	cube[0]->Render();
	cube[1]->Render();

	renderTarget[1]->Set();

	plane->SetShader(depthShader);
	sphere->SetShader(depthShader);
	cube[0]->SetShader(depthShader);
	cube[1]->SetShader(depthShader);

	plane->Render();
	sphere->Render();
	cube[0]->Render();
	cube[1]->Render();

}

void TestDepth::Render()
{

}

void TestDepth::PostRender()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	postEffect->Position(0, 0);
	postEffect->Scale(desc.Width * 0.5f, desc.Height);
	postEffect->SRV(renderTarget[0]->GetSRV());
	postEffect->Update();
	postEffect->Render();

	postEffect->Position(desc.Width * 0.5f, 0);
	postEffect->Scale(desc.Width * 0.5f, desc.Height);
	postEffect->SRV(renderTarget[1]->GetSRV());
	postEffect->Update();
	postEffect->Render();
}

