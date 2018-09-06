#include "stdafx.h"
#include "TestRenderTarget.h"

#include "../LandScape/Sky.h"

#include "../Objects/MeshPlane.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/MeshCube.h"

#include "../PostEffects/Grayscale.h"
#include "../PostEffects/Blur.h"
#include "../PostEffects/Mosaic.h"
#include "../PostEffects/PostEffects.h"

TestRenderTarget::TestRenderTarget(ExecuteValues * values)
	:Execute(values)
	, isRenderMesh(false)
{
	//sky = new Sky(values);

	shader = new Shader(Shaders + L"019_NormalMap.hlsl");

	renderTarget = new RenderTarget(values);

	// Create Render2D
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		render2D = new Render2D(values);
		render2D->Position(0, desc.Height * 0.5f);
		render2D->Scale(desc.Width * 0.5f, desc.Height * 0.5f);

		//postEffect = new Grayscale(values);
		//postEffect = new Blur(values);
		//postEffect = new Mosaic(values);
		postEffect = new PostEffects(values);

		postEffect->Position(desc.Width * 0.5f, 0);
		postEffect->Scale(desc.Width * 0.5f, desc.Height);
	}

	// Create Model
	{
		portrait = new MeshPlane();
		portrait->Scale(3, 3, 3);
		portrait->SetShader(shader);
		portrait->SetDiffuse(1, 1, 1, 1);
		portrait->SetDiffuseMap(Textures + L"Homework/Me.png");
	}

	// Create Mesh
	{
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
}

TestRenderTarget::~TestRenderTarget()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(portrait);
	SAFE_DELETE(sphere);
	SAFE_DELETE(plane);
	SAFE_DELETE(cube[0]);
	SAFE_DELETE(cube[1]);

	//SAFE_DELETE(sky);

	SAFE_DELETE(renderTarget);
	SAFE_DELETE(render2D);
	SAFE_DELETE(postEffect);
}

void TestRenderTarget::Update()
{
	//sky->Update();

	if (isRenderMesh)
	{
		plane->Update();
		sphere->Update();
		cube[0]->Update();
		cube[1]->Update();
	}
	else
		portrait->Update();
}

void TestRenderTarget::PreRender()
{
	renderTarget->Set();

	//sky->Render();

	if (isRenderMesh)
	{
		plane->Render();
		sphere->Render();
		cube[0]->Render();
		cube[1]->Render();
	}
	else
		portrait->Render();

	//if (Keyboard::Get()->Down(VK_SPACE))
	//	renderTarget->SaveRtvTexture(L"Test.png");
}

void TestRenderTarget::Render()
{

}

void TestRenderTarget::PostRender()
{
	ImGui::Checkbox("Render Mesh", &isRenderMesh);

	if (ImGui::Button("Load Texture")) {
		func = bind(&TestRenderTarget::LoadTexture, this, placeholders::_1);
		Path::OpenFileDialog(L"", L"*.png",
			Textures + L"Homework", func);
	}


	ImGui::Begin("Post Effect");

	render2D->Update();
	render2D->SRV(renderTarget->GetSRV());
	render2D->Render();

	postEffect->Update();
	postEffect->SRV(renderTarget->GetSRV());
	postEffect->Render();

	ImGui::End();
}

void TestRenderTarget::LoadTexture(wstring fileName)
{
	portrait->SetDiffuseMap(fileName);
}
