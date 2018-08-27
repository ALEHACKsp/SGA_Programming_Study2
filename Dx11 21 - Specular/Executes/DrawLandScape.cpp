#include "stdafx.h"
#include "DrawLandScape.h"

#include "../LandScape/Sky.h"
#include "../LandScape/Terrain.h"
#include "../Objects/MeshSphere.h"
#include "../Objects/Billboard.h"

DrawLandScape::DrawLandScape(ExecuteValues * values)
	:Execute(values)
{
	// Create Sky
	{
		sky = new Sky(values);
	}

	// Create Terrain
	{
		wstring heightMap = Contents + L"HeightMaps/HeightMap256.png";
		Material * material = new Material(Shaders + L"016_TerrainBrushLine.hlsl");

		terrain = new Terrain(values, material);
	}

	// Create MeshSphere
	{
		specular = new Shader(Shaders + L"018_Specular.hlsl");

		diffuseColor = D3DXCOLOR(1, 0, 0, 1);
		specularColor = D3DXCOLOR(1, 1, 1, 1);
		shininess = 15.0f;

		sphere = new MeshSphere();

		sphere->SetShader(specular);
		sphere->SetDiffuseMap(Textures + L"Wall.png");
		sphere->SetSpecularMap(Textures + L"Wall_specular.png");

		D3DXMATRIX S;
		D3DXMatrixScaling(&S, 5, 5, 5);
		sphere->RootAxis(S);

		sphere->Position(D3DXVECTOR3(80, 10, 30));
	}
}

DrawLandScape::~DrawLandScape()
{
	for (Billboard * tree : trees)
		SAFE_DELETE(tree);

	SAFE_DELETE(sphere);
	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);
}

void DrawLandScape::Update()
{
	D3DXVECTOR3 position = sphere->Position();

	if (Keyboard::Get()->Press('I'))
		position.z += 10.0f * Time::Delta();
	else if (Keyboard::Get()->Press('K'))
		position.z -= 10.0f * Time::Delta();

	if (Keyboard::Get()->Press('J'))
		position.x -= 10.0f * Time::Delta();
	else if (Keyboard::Get()->Press('L'))
		position.x += 10.0f * Time::Delta();

	// 1.5f ũ�� �÷����� �� ���ذ� �߽��� ������ �ٵ� sphere �ƴ�
	//position.y = terrain->Y(position); 
	
	// �浹 ��� ���
	D3DXVECTOR3 newPosition;
	if (terrain->Y(&newPosition, position) == true)
		position.y = newPosition.y;

	sphere->Position(position);

	sky->Update();
	terrain->Update();
	sphere->Update();

	for (Billboard * tree : trees)
		tree->Update();
}

void DrawLandScape::PreRender()
{

}

void DrawLandScape::Render()
{
	sky->Render();
	terrain->Render();
	sphere->Render();

	for (Billboard * tree : trees)
		tree->Render();
}

void DrawLandScape::PostRender()
{
	ImGui::Separator();

	ImGui::SliderFloat3("Diffuse", (float *)&diffuseColor, 0, 1);
	ImGui::SliderFloat3("Specular", (float *)&specularColor, 0, 1);
	ImGui::SliderFloat("Shininess", &shininess, 0.1f, 30.0f);

	ImGui::Separator();

	sphere->SetDiffuse(diffuseColor);
	sphere->SetSpecular(specularColor);
	sphere->SetShininess(shininess);

	if (terrain->GetBrushBuffer()->Data.Type > 0)
	{
		D3DXVECTOR3 picked;
		terrain->Y(&picked);

		ImGui::LabelText("Picked", "%.2f, %.2f, %.2f",
			picked.x, picked.y, picked.z);
	}
}

void DrawLandScape::ResizeScreen()
{
}

