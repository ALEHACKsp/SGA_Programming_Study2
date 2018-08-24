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
		sphere = new MeshSphere();

		D3DXMATRIX S;
		D3DXMatrixScaling(&S, 3, 3, 3);
		sphere->RootAxis(S);

		sphere->Position(D3DXVECTOR3(80, 10, 30));
	}

	// Create Trees
	{
		for (int i = 0; i < 20; i++)
		{
			Billboard* tree = new Billboard(values,
				Shaders + L"017_Billboard.hlsl", Textures + L"Tree.png");

			D3DXVECTOR3 P;
			P.x = Math::Random(0.0f, 255.0f);
			P.z = Math::Random(0.0f, 255.0f);
			P.y = terrain->Y(P);

			D3DXVECTOR3 S;
			S.x = Math::Random(10.0f, 30.0f);
			S.y = Math::Random(10.0f, 30.0f);
			S.z = 1.0f;

			P.y += S.y * 0.5f;

			tree->Position(P);
			tree->Scale(S);

			bool fixedY = Math::Random(0, 1) == 0 ? false : true;
			tree->FixedY(fixedY);

			trees.push_back(tree);
		}
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

	// 1.5f 크기 늘려놔서 더 해준거 중심이 가운데라면 근데 sphere 아님
	//position.y = terrain->Y(position); 
	
	// 충돌 계산 방법
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

