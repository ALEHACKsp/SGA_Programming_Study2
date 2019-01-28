#include "stdafx.h"
#include "TestPlane.h"

#include "Fbx\FbxLoader.h"

void TestPlane::Initialize()
{

}

void TestPlane::Ready()
{
	wstring shaderFile = Shaders + L"055_Mesh.fx";

	gridMaterial = new Material(shaderFile);
	gridMaterial->SetDiffuseMap(Textures + L"Floor.png");
	gridMaterial->SetAmbient(0.8f, 0.8f, 0.8f);
	gridMaterial->SetDiffuse(0.8f, 0.8f, 0.8f);
	gridMaterial->SetSpecular(1.0f, 1.0f, 1.0f, 16.0f);

	grid = new MeshGrid(gridMaterial, 32, 32, 10, 10);
}

void TestPlane::Destroy()
{
	SAFE_DELETE(gridMaterial);
	SAFE_DELETE(grid);
}

void TestPlane::Update()
{

}

void TestPlane::PreRender()
{
}

void TestPlane::Render()
{
	grid->Render();
}

void TestPlane::PostRender()
{
}

void TestPlane::ResizeScreen()
{
}
