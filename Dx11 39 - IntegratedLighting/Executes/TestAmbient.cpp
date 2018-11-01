#include "stdafx.h"
#include "TestAmbient.h"

#include "../Objects/MeshPlane.h"
#include "../Objects/MeshCube.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/MeshBunny.h"

TestAmbient::TestAmbient(ExecuteValues * values)
	:Execute(values)
{
	shader = new Shader(Shaders + L"038_Lighting.hlsl");

	// Mesh
	{
		plane = new MeshPlane();
		plane->Scale(10, 1, 10);
		plane->SetShader(shader);
		plane->SetDiffuse(1, 1, 1);

		cube = new MeshCube();
		cube->Position(-10, 1.5f, 0);
		cube->SetShader(shader);
		cube->SetDiffuse(0, 1, 0);
		cube->Scale(3, 3, 3);

		sphere = new MeshSphere();
		sphere->Position(10, 1.5f, 0);
		sphere->SetShader(shader);
		sphere->SetDiffuse(0, 0, 1);
		sphere->Scale(3, 3, 3);

		sphere2 = new MeshSphere();
		sphere2->Position(10, 1.5f, 10);
		sphere2->SetShader(shader);
		sphere2->SetDiffuse(1, 1, 1);
		sphere2->Scale(3, 3, 3);

		bunny = new MeshBunny();
		bunny->SetShader(shader);
		bunny->SetDiffuse(1, 1, 1);
		bunny->Scale(0.02f, 0.02f, 0.02f);
		bunny->Position(0, 5, 0);
		bunny->SetDiffuseMap(Textures + L"White.png");
		bunny->SetShininess(20);
	}

	// Light Setting
	{
		lightBuffer = new LightBuffer();

		LightData data;
		{
			LightDesc desc;
			desc.Type = ELightType::Capsule;
			desc.Position = D3DXVECTOR3(0, 10, 0);
			desc.Direction = D3DXVECTOR3(1, 0, 0);
			desc.Range = 50;
			desc.Length = 80;
			desc.Color = D3DXVECTOR3(1, 0, 0);

			data.Setting(0, desc);

			desc.Type = ELightType::Capsule;
			desc.Position = D3DXVECTOR3(0, 10, 0);
			desc.Direction = D3DXVECTOR3(-1, 0, 0);
			desc.Range = 50;
			desc.Length = 80;
			desc.Color = D3DXVECTOR3(0, 1, 0);
			data.Setting(1, desc);

			desc.Type = ELightType::Capsule;
			desc.Position = D3DXVECTOR3(0, 10, 0);
			desc.Direction = D3DXVECTOR3(0, 0, 1);
			desc.Range = 50;
			desc.Length = 80;
			desc.Color = D3DXVECTOR3(0, 0, 1);
			data.Setting(2, desc);

			desc.Type = ELightType::Capsule;
			desc.Position = D3DXVECTOR3(0, 10, 0);
			desc.Direction = D3DXVECTOR3(0, 0, -1);
			desc.Range = 50;
			desc.Length = 80;
			desc.Color = D3DXVECTOR3(1, 1, 0);
			data.Setting(3, desc);
		}
		lightBuffer->AddLight(data);
	}
	
}

TestAmbient::~TestAmbient()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(lightBuffer);

	SAFE_DELETE(plane);
	SAFE_DELETE(cube);
	SAFE_DELETE(sphere);
	SAFE_DELETE(sphere2);

	SAFE_DELETE(bunny);
}

void TestAmbient::Update()
{
	plane->Update();
	cube->Update();
	sphere->Update();
	sphere2->Update();

	bunny->Update();
}

void TestAmbient::PreRender()
{

}

void TestAmbient::Render()
{
	lightBuffer->SetPSBuffer(2);

	plane->Render();
	cube->Render();
	sphere->Render();
	sphere2->Render();
	
	bunny->Render();
}

void TestAmbient::PostRender()
{

}



