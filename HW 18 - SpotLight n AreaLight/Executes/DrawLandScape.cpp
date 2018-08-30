#include "stdafx.h"
#include "DrawLandScape.h"

#include "../LandScape/Sky.h"
#include "../LandScape/Terrain.h"

#include "../Objects/MeshSphere.h"
#include "../Objects/MeshCube.h"
#include "../Objects/Billboard.h"

#include "../Objects/PointLight.h"
#include "../Objects/SpotLight.h"
#include "../Objects/AreaLight.h"

DrawLandScape::DrawLandScape(ExecuteValues * values)
	:Execute(values)
	, selectModel(-1), pickPos(0, 0, 0)
{
	// Create Sky
	{
		sky = new Sky(values);
	}

	// Create Terrain
	{
		wstring heightMap = Contents + L"HeightMaps/HeightMap256.png";
		Material * material = new Material(Shaders + L"Homework/TerrainBrushLine.hlsl");
	
		terrain = new Terrain(values, material);
	}

	// Create Light
	{
		pointLight = new PointLight();
		spotLight = new SpotLight();
		areaLight = new AreaLight();
	}

	normalBuffer = new NormalBuffer();

	moveRange[0] = 0;
	moveRange[1] = 0;
	moveRange[2] = 256;
	moveRange[3] = 256;
}

DrawLandScape::~DrawLandScape()
{
	SAFE_DELETE(areaLight);
	SAFE_DELETE(spotLight);
	SAFE_DELETE(pointLight);

	SAFE_DELETE(normalBuffer);

	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);

	for (GameModel* model : models)
		SAFE_DELETE(model);
}

void DrawLandScape::Update()
{
	areaLight->Update();
	spotLight->Update();
	pointLight->Update();
	

	for (int i = 0; i < isRandomMoves.size(); i++)
	{
		if (isRandomMoves[i].isMove == true) {
			spotLight->GetByIndex(i)->Position += isRandomMoves[i].dir 
				* 30.0f * Time::Delta();
			isRandomMoves[i].liveTime -= Time::Delta();

			D3DXVECTOR3 pos = spotLight->GetByIndex(i)->Position;

			if (isRandomMoves[i].liveTime < 0 
				|| pos.x < moveRange[0] || pos.x >= moveRange[2] 
				|| pos.z < moveRange[1] || pos.z >= moveRange[3])
			{
				isRandomMoves[i].dir = D3DXVECTOR3(
					Math::Random(-1.0f, 1.0f), 0, Math::Random(-1.0f, 1.0f));
				isRandomMoves[i].liveTime = 1.0f;

				if (spotLight->GetByIndex(i)->Position.x < moveRange[0])
					spotLight->GetByIndex(i)->Position.x = moveRange[0];
				if (spotLight->GetByIndex(i)->Position.x > moveRange[2])
					spotLight->GetByIndex(i)->Position.x = moveRange[2];
				if (spotLight->GetByIndex(i)->Position.z < moveRange[1])
					spotLight->GetByIndex(i)->Position.z = moveRange[1];
				if (spotLight->GetByIndex(i)->Position.z > moveRange[3])
					spotLight->GetByIndex(i)->Position.z = moveRange[3];

			}
		}
	}

	sky->Update();
	terrain->Update();

	for (GameModel* model : models)
		model->Update();

	// mainCamera orbit
	if (models.size() != 0)
	{
		// orbit Cam select
		GameModel* model = models.back();
		if (selectModel != -1 && selectModel < models.size())
			model = models[selectModel];
		values->MainCamera->SetTarget(model->Position());
	
		// model orbit select
		for (int i = 0; i < models.size(); i++) {
			if (*models[i]->GetIsOrbit() == false && i == 0) continue;
	
			int* targetNum = models[i]->GetTargetNum();
			models[i]->SetTarget(models[*targetNum]->Position());
		}
	}
}

void DrawLandScape::PreRender()
{

}

void DrawLandScape::Render()
{
	sky->Render();
	terrain->Render();

	normalBuffer->SetPSBuffer(10);
	for (GameModel* model : models)
		model->Render();
}

void DrawLandScape::PostRender()
{
	sky->PostRender();
	terrain->PostRender();
	
	// Model
	{
		// Create Menu
		ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar())
		{
			// Create Model
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Sphere", "Create Sphere"))
				{
					models.push_back(new MeshSphere());
					models.back()->Position(pickPos.x, pickPos.y + 0.5f, pickPos.z);
					//models.back()->SetDiffuseMap(Textures + L"Box.png");
					//models.back()->SetSpecularMap(Textures + L"Box_specular.png");
				}
				if (ImGui::MenuItem("Cube", "Create Cube"))
				{
					models.push_back(new MeshCube());
					models.back()->Position(pickPos.x, pickPos.y + 0.5f, pickPos.z);
				}
				if (ImGui::MenuItem("PointLight", "Create Point Light"))
				{
					PointLight::Desc desc;

					desc.Position = pickPos;
					desc.Color = D3DXVECTOR3(1, 1, 1);
					desc.Intensity = 0.1f;
					desc.Range = 5;
					pointLight->Add(desc);
				}
				if (ImGui::MenuItem("SpotLight", "Create Spot Light"))
				{
					SpotLight::Desc desc;

					desc.Position = pickPos + D3DXVECTOR3(0,10,0);
					desc.Color = D3DXVECTOR3(1, 1, 1);
					desc.InnerAngle = 0.7f;
					desc.OuterAngle = 1.1f;
					desc.Direction = D3DXVECTOR3(0, -1, 0);
					spotLight->Add(desc);

					IsRandomMove temp;
					temp.isMove = false;
					temp.dir = D3DXVECTOR3(Math::Random(-1.0f, 1.0f), 0, Math::Random(-1.0f, 1.0f));
					temp.liveTime = 1.0f;
					isRandomMoves.push_back(temp);
				}
				if (ImGui::MenuItem("AreaLight", "Create Area Light"))
				{
					AreaLight::Desc desc;

					desc.Position = pickPos + D3DXVECTOR3(0, 10, 0);
					desc.Color = D3DXVECTOR3(1, 1, 1);
					desc.HalfSize = D3DXVECTOR3(10, desc.Position.y * 2, 10);
					areaLight->Add(desc);
				}
				ImGui::EndMenu();
			}
			// Delete Model
			if (ImGui::BeginMenu("Delete"))
			{
				if (ImGui::MenuItem("Delete Last", "Delete Last Model"))
				{
					if(models.size() > 0)
						models.pop_back();
				}
				if (ImGui::MenuItem("Delete All", "Delete All Model"))
				{
					models.clear();
				}
				if (ImGui::MenuItem("Delete Last PointLight", "Delete Last PointLight"))
				{
					pointLight->LastDelete();
				}
				if (ImGui::MenuItem("Delete Last SpotLight", "Delete Last SpotLight"))
				{
					spotLight->LastDelete();
					isRandomMoves.pop_back();
				}
				if (ImGui::MenuItem("Delete Last AreaLight", "Delete Last AreaLight"))
				{
					areaLight->LastDelete();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Pick Model
		if (Mouse::Get()->Down(0)
			&& Keyboard::Get()->Press(VK_LSHIFT))
		{
			for (int i = 0; i < models.size(); i++)
			{
				GameModel* model = models[i];
				if (model->IsPickModel(
					values->MainCamera, values->Viewport, values->Perspective))
				{
					selectModel = i;
					break;
				}
				else
					selectModel = -1;
			}
		}

		// Hierarchy
		int start = 0;
		int end = models.size();
		for (int i = start; i < end; i++)
		{
			GameModel* model = models[i];
			ImGui::RadioButton((model->GetName() + " " + to_string(i)).c_str(),
					&selectModel, i);
		}
		ImGui::Separator();

		start = end;
		end += pointLight->Count();
		for (int i = start; i < end; i++) {
			ImGui::RadioButton(("PointLight " + to_string(i - start)).c_str(),
				&selectModel, i);
		}

		start = end;
		end += spotLight->Count();
		for (int i = start; i < end; i++) {
			ImGui::RadioButton(("SpotLight " + to_string(i - start)).c_str(),
				&selectModel, i);
		}

		start = end;
		end += areaLight->Count();
		for (int i = start; i < end; i++) {
			ImGui::RadioButton(("AreaLight " + to_string(i - start)).c_str(),
				&selectModel, i);
		}

		ImGui::End();

		// Model Inspector
		ImGui::Begin("Inspector");
		for (int i = 0; i < models.size(); i++)
		{
			GameModel* model = models[i];
			if (i == selectModel) {
				model->PostRender();

				Texture* texture = model->GetModel()->MaterialByIndex(0)->GetDiffuseMap();

				ImGui::Text("DiffuseMap");
				if (ImGui::ImageButton(texture != NULL ? texture->GetView() : 0
					, ImVec2(100, 100)))
				{
					func = bind(&GameModel::SetDiffuseMap, model, placeholders::_1);
					Path::OpenFileDialog(L"", L"PNG Files(*.png)\0*.png\0", Textures, func);
				}
				ImGui::SameLine(130);
				if (ImGui::Button("Delete DiffuseMap"))
					model->DeleteDiffuseMap();

				ImGui::Text("SpecularMap");
				texture = model->GetModel()->MaterialByIndex(0)->GetSpecularMap();
				if (ImGui::ImageButton(texture != NULL ? texture->GetView() : 0
					, ImVec2(100, 100)))
				{
					func = bind(&GameModel::SetSpecularMap, model, placeholders::_1);
					Path::OpenFileDialog(L"", L"PNG Files(*.png)\0*.png\0", Textures, func);
				}
				ImGui::SameLine(130);
				if (ImGui::Button("Delete SpecularMap"))
					model->DeleteSpecularMap();

				ImGui::SliderInt("Visible", &normalBuffer->Data.VisibleNormal, 0, 1);

				ImGui::Text("NormalMap");
				texture = model->GetModel()->MaterialByIndex(0)->GetNormalMap();
				if (ImGui::ImageButton(texture != NULL ? texture->GetView() : 0
					, ImVec2(100, 100)))
				{
					func = bind(&GameModel::SetNormalMap, model, placeholders::_1);
					Path::OpenFileDialog(L"", L"PNG Files(*.png)\0*.png\0", Textures, func);
				}
				ImGui::SameLine(130);
				if (ImGui::Button("Delete NormalMap"))
					model->DeleteNormalMap();

				// 0번 sphere는 공전 안하게 고정
				if (i != 0)
				{
					// Orbit
					ImGui::Separator();
					ImGui::Checkbox("Orbit", model->GetIsOrbit());
					ImGui::DragFloat("Orbit Dist", model->GetOrbitDist(), 0.1f, 1.0f);
					//ImGui::DragFloat3("Orbit Dist", (float*)model->GetOrbitDist(), 0.1f, 1.0f);
				
					int* targetNum = model->GetTargetNum();
				
					ImGui::SliderInt("Select Target", targetNum, 0, models.size() - 1);
					if (*targetNum == selectModel)
						*targetNum = 0;
				}
				break;
			}
		}
		if (selectModel >= models.size())
		{
			int temp = models.size();
			for (int i = 0; i < pointLight->Count(); i++)
			{
				if (selectModel - temp == i)
					pointLight->PostRender(i);
			}
			
			temp += pointLight->Count();
			for (int i = 0; i < spotLight->Count(); i++)
			{
				if (selectModel - temp == i) {
					spotLight->PostRender(i);
					bool b = isRandomMoves[i].isMove;
					ImGui::Checkbox("Random Move", &b);
					isRandomMoves[i].isMove = b;
					ImGui::InputFloat4("Random Move Range", moveRange);
				}
			}

			temp += spotLight->Count();
			for (int i = 0; i < areaLight->Count(); i++)
			{
				if (selectModel - temp == i)
					areaLight->PostRender(i);
			}
		}

		ImGui::End();
	}

	ImGui::LabelText("Picked", "%.2f, %.2f, %.2f",
		pickPos.x, pickPos.y, pickPos.z);

	if (Keyboard::Get()->Press(VK_LSHIFT))
	{
		D3DXVECTOR3 picked;
		bool check = terrain->Y(&picked);
		if (check == false)
			picked = { 0,0,0 };

		if (Mouse::Get()->Press(1))
		{
			pickPos = picked;
		}
	}
}

void DrawLandScape::ResizeScreen()
{
}

