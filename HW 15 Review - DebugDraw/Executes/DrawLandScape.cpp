#include "stdafx.h"
#include "DrawLandScape.h"

#include "../LandScape/Sky.h"
#include "../LandScape/Terrain.h"

#include "../Objects/MeshSphere.h"
#include "../Objects/Billboard.h"
#include "../Objects/GameTank.h"

#include "../Utilities/Memory.h"

DrawLandScape::DrawLandScape(ExecuteValues * values)
	:Execute(values)
	, selectModel(-1), modelKind(MODEL_KIND_TREE)
	, treeCnt(0)
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

	//// Create Tank
	//{
	//	models.push_back(new GameTank(Models + L"Tank/", L"Tank.material"));
	//}
	
	// Create Trees
	{
		//for (int i = 0; i < 20; i++)
		//{
		//	Billboard* tree = new Billboard(values,
		//		Shaders + L"017_Billboard.hlsl", Textures + L"Tree.png");
		//
		//	D3DXVECTOR3 P;
		//	P.x = Math::Random(0.0f, 255.0f);
		//	P.z = Math::Random(0.0f, 255.0f);
		//	P.y = terrain->Y(P);
		//
		//	D3DXVECTOR3 S;
		//	S.x = Math::Random(10.0f, 30.0f);
		//	S.y = Math::Random(10.0f, 30.0f);
		//	S.z = 1.0f;
		//
		//	P.y += S.y * 0.5f;
		//
		//	tree->Position(P);
		//	tree->Scale(S);
		//
		//	bool fixedY = Math::Random(0, 1) == 0 ? false : true;
		//	tree->FixedY(fixedY);
		//
		//	models.push_back(tree);
		//}

		//models.push_back(new Billboard(values,
		//	Shaders + L"017_Billboard.hlsl", Textures + L"Tree.png"));

		//D3DXVECTOR3 S, P;
		//P = models.back()->Position();
		//P.y += 0.5f;

		//models.back()->Position(P);
	}

	//// Create Sphere
	//{
	//	models.push_back(new MeshSphere());
	//}
}

DrawLandScape::~DrawLandScape()
{
	for (GameModel * model : models)
		SAFE_DELETE(model);

	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);
}

void DrawLandScape::Update()
{

	sky->Update();
	terrain->Update();

	for (GameModel * model : models) {
		if (model->GetIsLive() == false) continue;
		model->Update();
	}
}

void DrawLandScape::PreRender()
{

}

void DrawLandScape::Render()
{
	sky->Render();
	terrain->Render();

	for (GameModel * model : models) 
	{
		if (model->GetIsLive() == false) continue;
		model->Render();
	}
}

void DrawLandScape::PostRender()
{
	sky->PostRender();
	terrain->PostRender();

	if (terrain->GetBrushBuffer()->Data.Type > 0)
	{
		D3DXVECTOR3 picked; 
		terrain->Y(&picked);
	
		ImGui::LabelText("Picked", "%.2f, %.2f, %.2f",
			picked.x, picked.y, picked.z);
	}

	if (Mouse::Get()->Down(0))
	{
		// Create Model
		if (Keyboard::Get()->Press(VK_LCONTROL))
		{
			D3DXVECTOR3 picked;
			terrain->Y(&picked);

			switch (modelKind)
			{
			case DrawLandScape::MODEL_KIND_TREE:
			{
				models.push_back(new Billboard(values,
					Shaders + L"017_Billboard.hlsl", Textures + L"Tree.png"));

				D3DXVECTOR3 scale = { 10, 10, 1 };
				picked.y += scale.y * 0.5f;
				models.back()->Scale(scale);
				models.back()->Position(picked);
				models.back()->SetName("Tree " + to_string(treeCnt++));

				models.back()->SetIsMemory(true);
				models.back()->SetIsLive(true);
				models.back()->SetIsMemory(true);
			}
				break;
			case DrawLandScape::MODEL_KIND_TANK:
			{
				models.push_back(new GameTank(Models + L"Tank/", L"Tank.material"));
				models.back()->Position(picked);

				models.back()->SetIsMemory(true);
				models.back()->SetIsLive(true);
				models.back()->SetIsMemory(true);
			}
			break;
			case DrawLandScape::MODEL_KIND_SPHERE:
			{	
				models.push_back(new MeshSphere());

				D3DXVECTOR3 scale = models.back()->Scale();
				picked.y += scale.y * 0.5f;
				models.back()->Scale(scale);
				models.back()->Position(picked);

				models.back()->SetIsMemory(true);
				models.back()->SetIsLive(true);
				models.back()->SetIsMemory(true);
			}
			break;
			}
		}

		// Select Model
		if (Keyboard::Get()->Press(VK_LSHIFT))
		{
			for (UINT i = 0; i < models.size(); i++)
			{
				if (models[i]->IsPickModel(
					values->MainCamera, values->Viewport, values->Perspective))
				{
					selectModel = i;
					break;
				}
				else
					selectModel = -1;
			}
		}
	}

	// Model Info
	ImGui::Begin("Model Info");

	if (ImGui::Button("Undo"))
		Memory::UnDo();
	ImGui::SameLine(50);
	if (ImGui::Button("Redo"))
		Memory::ReDo();
	ImGui::Separator();

	ImGui::Text("Model Kind");
	ImGui::RadioButton("Tree", (int*)&modelKind, MODEL_KIND_TREE);
	ImGui::SameLine(80);
	ImGui::RadioButton("Tank", (int*)&modelKind, MODEL_KIND_TANK);
	ImGui::SameLine(160);
	ImGui::RadioButton("Sphere", (int*)&modelKind, MODEL_KIND_SPHERE);

	ImGui::Separator();
	for (UINT i = 0; i < models.size(); i++)
	{
		GameModel * model = models[i];
		if (model->GetIsLive() == false) continue;

		if (selectModel == i)
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1),
				("Model : " + model->GetName()).c_str());
			ImGui::SameLine(260);
			if (ImGui::Button("Delete"))
			{
				model->SetIsLive(false);
			}

			ImGui::Text("Transform");

			D3DXVECTOR3 S, R, T;
			S = model->Scale();
			R = model->Rotation();
			T = model->Position();

			float s[] = { S.x,S.y,S.z };
			float r[] = { R.x,R.y,R.z };
			float t[] = { T.x,T.y,T.z };

			ImGui::Text("Position");
			ImGui::SameLine(80);
			ImGui::DragFloat3(" T", t, 0.1f);

			ImGui::Text("Rotation");
			ImGui::SameLine(80);
			ImGui::DragFloat3(" R", r, 0.01f);

			ImGui::Text("Scale");
			ImGui::SameLine(80);
			ImGui::DragFloat3(" S", s, 0.01f);
			
			model->Scale(s[0],s[1],s[2]);
			model->Rotation(r[0],r[1],r[2]);
			model->Position(t[0],t[1],t[2]);

			if (model->GetName().find("Tree") != string::npos)
			{
				ImGui::Checkbox("IsDouble", ((Billboard*)model)->GetisDouble());
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1),
				("Model : " + model->GetName()).c_str());
		}

		ImGui::Separator();

		if (Mouse::Get()->Up(0))
			model->SetIsMemory(true);
	}
	ImGui::End();

	// Memory
	Memory::PostRender();
}

void DrawLandScape::ResizeScreen()
{
}

