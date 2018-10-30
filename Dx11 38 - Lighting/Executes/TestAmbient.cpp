#include "stdafx.h"
#include "TestAmbient.h"

#include "../Objects/MeshPlane.h"
#include "../Objects/MeshCube.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/MeshBunny.h"

TestAmbient::TestAmbient(ExecuteValues * values)
	:Execute(values)
{
	shader = new Shader(Shaders + L"037_Lighting.hlsl");

	ambientBuffer = new AmbientBuffer();
	specularBuffer = new SpecularBuffer();
	pointLightBuffer = new PointLightBuffer();
	spotLightBuffer = new SpotLightBuffer();
	capsuleLightBuffer = new CapsuleLightBuffer();

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
}

TestAmbient::~TestAmbient()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(ambientBuffer);
	SAFE_DELETE(specularBuffer);
	SAFE_DELETE(pointLightBuffer);
	SAFE_DELETE(spotLightBuffer);
	SAFE_DELETE(capsuleLightBuffer);

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
	//ImGui::Separator();
	//ImGui::SliderFloat3("AmbientFloor", (float*)&ambientBuffer->Data.Floor, 0, 1);
	//ImGui::SliderFloat3("AmbientCeil", (float*)&ambientBuffer->Data.Ceil, 0, 1);

	//ImGui::Separator();
	//ImGui::ColorEdit3("DirectionColor", (float*)&ambientBuffer->Data.Color);

	//ImGui::Separator();
	//ImGui::SliderFloat("Specular Exp", &specularBuffer->Data.Exp, 1, 100);
	//ImGui::SliderFloat("Specular Intensity", &specularBuffer->Data.Intensity, 0, 10);

	//ImGui::Separator();
	//ImGui::SliderFloat3("PointLight Position", (float*)&pointLightBuffer->Data.Position, -50, 50);
	//ImGui::SliderFloat("PointLight Range", &pointLightBuffer->Data.Range, 1, 100);
	//ImGui::ColorEdit3("PointLight Color", (float*)&pointLightBuffer->Data.Color);

	//ImGui::Separator();
	//ImGui::SliderFloat3("SpotLight Position", (float*)&spotLightBuffer->Data.Position, -50, 50);
	//ImGui::SliderFloat("SpotLight Range", &spotLightBuffer->Data.Range, 1, 100);
	//ImGui::ColorEdit4("SpotLight Color", (float*)&spotLightBuffer->Data.Color);
	//ImGui::SliderFloat3("SpotLight Direction", (float*)&spotLightBuffer->Data.Direction, -1, 1);

	//ImGui::SliderFloat("SpotLight Inner", &spotLightBuffer->Data.Inner, 0, 90);
	//ImGui::SliderFloat("SpotLight Outer", &spotLightBuffer->Data.Outer, 0, 90);

	ImGui::Separator();
	ImGui::SliderFloat3("CapsuleLight Position", (float*)&capsuleLightBuffer->Data.Position, -50, 50);
	ImGui::SliderFloat3("CapsuleLight Direction", (float*)&capsuleLightBuffer->Data.Direction, -1, 1);
	ImGui::SliderFloat("CapsuleLight Range", &capsuleLightBuffer->Data.Range, 1, 100);
	ImGui::SliderFloat("CapsuleLight Length", &capsuleLightBuffer->Data.Length, 0, 100);
	ImGui::ColorEdit4("CapsuleLight Color", (float*)&capsuleLightBuffer->Data.Color);

	ImGui::Separator();

	ambientBuffer->SetPSBuffer(10);
	specularBuffer->SetPSBuffer(11);
	pointLightBuffer->SetPSBuffer(12);
	spotLightBuffer->SetPSBuffer(13);
	capsuleLightBuffer->SetPSBuffer(9);

	plane->Render();
	cube->Render();
	sphere->Render();
	sphere2->Render();
	
	bunny->Render();
}

void TestAmbient::PostRender()
{

}



