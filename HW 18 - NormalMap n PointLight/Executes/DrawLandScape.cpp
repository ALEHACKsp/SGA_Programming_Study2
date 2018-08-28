#include "stdafx.h"
#include "DrawLandScape.h"

#include "../LandScape/Sky.h"
#include "../LandScape/Terrain.h"

#include "../Objects/MeshSphere.h"
#include "../Objects/MeshCube.h"
#include "../Objects/Billboard.h"
#include "../Objects/PointLight.h"

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

	// Create PointLight
	{
		pointLight = new PointLight();
		//PointLight::Desc desc;

		//desc.Position = D3DXVECTOR3(-5, 0, 0);
		//desc.Color = D3DXVECTOR3(1, 0, 0);
		//desc.Intensity = 0.1f;
		//desc.Range = 5;
		//pointLight->Add(desc);

		//desc.Position = D3DXVECTOR3(5, 0, 0);
		//desc.Color = D3DXVECTOR3(0, 0, 1);
		//desc.Intensity = 0.1f;
		//desc.Range = 5;
		//pointLight->Add(desc);
		//lines.push_back(new DebugDrawSphere(
		//	pointLight->GetByIndex(1)->Position,
		//	pointLight->GetByIndex(1)->Range));

	}

	//// Create MeshSphere
	//{
	//	specular = new Shader(Shaders + L"018_Specular.hlsl");
	//
	//	diffuseColor = D3DXCOLOR(1, 1, 1, 1);
	//	specularColor = D3DXCOLOR(1, 1, 1, 1);
	//	shininess = 15.0f;
	//
	//	sphere = new MeshSphere();
	//
	//	sphere->SetShader(specular);
	//	sphere->SetDiffuseMap(Textures + L"Wall.png");
	//	sphere->SetSpecularMap(Textures + L"Wall_specular.png");
	//
	//	D3DXMATRIX S;
	//	D3DXMatrixScaling(&S, 5, 5, 5);
	//	sphere->RootAxis(S);
	//
	//	sphere->Position(D3DXVECTOR3(80, 10, 30));
	//}

	//models.back()->SetDiffuse(1, 1, 1);
	//models.back()->SetSpecular(1, 1, 1);
	//models.back()->SetShininess(5.0f);

	normalBuffer = new NormalBuffer();
}

DrawLandScape::~DrawLandScape()
{
	SAFE_DELETE(pointLight);

	SAFE_DELETE(normalBuffer);

	//SAFE_DELETE(sphere);
	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);

	for (GameModel* model : models)
		SAFE_DELETE(model);
}

void DrawLandScape::Update()
{
	pointLight->Update();

	sky->Update();
	terrain->Update();
	//sphere->Update();

	for (GameModel* model : models)
		model->Update();

	//if (models.size() != 0)
	//{
	//	// orbit Cam select
	//	GameModel* model = models.back();
	//	if (selectModel != -1)
	//		model = models[selectModel];
	//	values->MainCamera->SetTarget(model->Position());
	//
	//	// model orbit select
	//	for (int i = 0; i < models.size(); i++) {
	//		if (*models[i]->GetIsOrbit() == false && i == 0) continue;
	//
	//		int* targetNum = models[i]->GetTargetNum();
	//		models[i]->SetTarget(models[*targetNum]->Position());
	//	}
	//}
}

void DrawLandScape::PreRender()
{

}

void DrawLandScape::Render()
{
	sky->Render();
	terrain->Render();
	//sphere->Render();

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
				//if (ImGui::MenuItem("Sphere", "Create Sphere"))
				//{
				//	models.push_back(new MeshSphere());
				//	//models.back()->SetDiffuseMap(Textures + L"Box.png");
				//	//models.back()->SetSpecularMap(Textures + L"Box_specular.png");
				//}
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
		for (int i = 0; i < models.size(); i++)
		{
			GameModel* model = models[i];
			ImGui::RadioButton((model->GetName() + " " + to_string(i)).c_str(),
					&selectModel, i);
		}
			ImGui::Separator();

		for (int i = models.size(); i < pointLight->Count() + models.size(); i++) {
			ImGui::RadioButton(("PointLight " + to_string(i - models.size())).c_str(),
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

				//// 0번 sphere는 공전 안하게 고정
				//if (i != 0)
				//{
				//	// Orbit
				//	ImGui::Separator();
				//	ImGui::Checkbox("Orbit", model->GetIsOrbit());
				//	ImGui::DragFloat("Orbit Dist", model->GetOrbitDist(), 0.1f, 1.0f);
				//	//ImGui::DragFloat3("Orbit Dist", (float*)model->GetOrbitDist(), 0.1f, 1.0f);
				//
				//	int* targetNum = model->GetTargetNum();
				//
				//	ImGui::SliderInt("Select Target", targetNum, 0, models.size() - 1);
				//	if (*targetNum == selectModel)
				//		*targetNum = 0;
				//}
				break;
			}
		}
		if (selectModel >= models.size())
		{
			for (int i = 0; i < pointLight->Count(); i++)
			{
				if (selectModel - models.size() == i)
					pointLight->PostRender(i);
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

