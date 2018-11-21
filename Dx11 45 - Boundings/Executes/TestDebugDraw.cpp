#include "stdafx.h"
#include "TestDebugDraw.h"

#include "../Draw/MeshCube.h"
#include "../Draw/MeshSphere.h"
#include "../Draw/MeshGrid.h"

TestDebugDraw::TestDebugDraw(ExecuteValues * values)
	:Execute(values)
{
	gridMaterial = new Material(Shaders + L"Homework/Mesh3.hlsl");
	gridMaterial->SetDiffuseMap(Textures + L"White.png");
	grid = new MeshGrid(gridMaterial, 10, 10, 10, 10);

	ray = new Ray();

	cubeMaterial = new Material(Shaders + L"Homework/Mesh3.hlsl");
	cubeMaterial->SetDiffuseMap(Textures + L"Red.png");
	cube = new MeshCube(cubeMaterial, 1, 1, 1);
	cube->Position(D3DXVECTOR3(3, 1, 0));
	cube->Scale(1, 2, 1);

	cubeBounding = new BBox();
	cubeBounding->Min = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
	cubeBounding->Max = D3DXVECTOR3(+0.5f, +0.5f, +0.5f);

	cubeLine = new DebugLine();


	sphereMaterial = new Material(Shaders + L"Homework/Mesh3.hlsl");
	sphereMaterial->SetDiffuseMap(Textures + L"Green.png");
	sphere = new MeshSphere(sphereMaterial, 2);
	sphere->Scale(D3DXVECTOR3(2, 2, 2));
	sphere->Position(D3DXVECTOR3(-2, 3, 0));


	sphereBounding = new BSphere();
	sphereBounding->Center = D3DXVECTOR3(0, 0, 0);
	sphereBounding->Radius = 2.0f;

	sphereLine = new DebugLine();
}

TestDebugDraw::~TestDebugDraw()
{
	SAFE_DELETE(grid);
	SAFE_DELETE(gridMaterial);

	SAFE_DELETE(ray);

	SAFE_DELETE(cube);
	SAFE_DELETE(cubeMaterial);
	SAFE_DELETE(cubeBounding);
	SAFE_DELETE(cubeLine);

	SAFE_DELETE(sphere);
	SAFE_DELETE(sphereMaterial);
	SAFE_DELETE(sphereBounding);
	SAFE_DELETE(sphereLine);

}

void TestDebugDraw::Update()
{
	D3DXMATRIX V, P;
	values->MainCamera->Matrix(&V);
	values->Perspective->GetMatrix(&P);

	D3DXVECTOR3 S;
	values->MainCamera->Position(&S);

	D3DXMATRIX W;
	cube->Matrix(&W);
	//values->Viewport->GetRay(ray, S, W, V, P);

	//cubeTest = cubeBounding->Intersect(ray, cubeResult);

	values->Viewport->GetRay(ray, S, V, P);
	cubeTest = cubeBounding->Intersect(ray, W, cubeResult);
	cubeLine->Draw(W, cubeBounding);


	sphere->Matrix(&W);
	//values->Viewport->GetRay(ray, S, W, V, P);

	//BSphere sphereCheck = sphereBounding->Transform(W);
	sphereTest = sphereBounding->Intersect(ray, W, sphereResult);
	//sphereTest = sphereBounding->Intersect(ray, sphereResult);
	sphereLine->Draw(W, sphereBounding);
}

void TestDebugDraw::PreRender()
{

}

void TestDebugDraw::Render()
{
	ImGui::Checkbox("Cube Testing", &cubeTest);
	ImGui::Checkbox("Sphere Testing", &sphereTest);

	D3DXVECTOR3 pos; 
	sphere->Position(&pos);
	if (ImGui::DragFloat3("Sphere Position", (float*)&pos, 0.1f))
		sphere->Position(pos);
	D3DXVECTOR3 rot;
	sphere->Rotation(&rot);
	if (ImGui::DragFloat3("Sphere Rotation", (float*)&rot, 0.1f))
		sphere->Rotation(rot);
	D3DXVECTOR3 scale;
	sphere->Scale(&scale);
	if (ImGui::DragFloat3("Sphere Scale", (float*)&scale, 0.1f))
		sphere->Scale(scale);

	cube->Position(&pos);
	if (ImGui::DragFloat3("Cube Position", (float*)&pos, 0.1f))
		cube->Position(pos);

	ImGui::Separator();
	ImGui::Text("cube result : %f", cubeResult);
	ImGui::Text("sphere reulst : %f", sphereResult);

	grid->Render();

	cubeLine->Render();
	cube->Render();

	sphereLine->Render();
	sphere->Render();
}

void TestDebugDraw::PostRender()
{

}

