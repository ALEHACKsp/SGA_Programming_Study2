#include "stdafx.h"
#include "TestBoneAnimation.h"

#include "../LandScape/Sky.h"

#include "../Objects/GameTank.h"

#include "../PostEffects/PostEffects.h"

#include "../Animations/AnimationClip.h"

TestBoneAnimation::TestBoneAnimation(ExecuteValues * values)
	:Execute(values)
{
	//sky = new Sky(values);

	shader = new Shader(Shaders + L"Homework/Model2.hlsl");

	//renderTarget = new RenderTarget(values);

	//// Create Render2D
	//{
	//	D3DDesc desc;
	//	D3D::GetDesc(&desc);
	//
	//	render2D = new Render2D(values);
	//	render2D->Position(0, desc.Height * 0.5f);
	//	render2D->Scale(desc.Width * 0.5f, desc.Height * 0.5f);
	//
	//	postEffect = new PostEffects(values);
	//
	//	postEffect->Position(desc.Width * 0.5f, 0);
	//	postEffect->Scale(desc.Width * 0.5f, desc.Height);
	//}

	// Create Tank
	{
		models.push_back(new GameTank(Models + L"Tank/", L"Tank.material"));
		models.back()->SetShader(shader);
	}

	selectModel = -1;

	//// test
	//deltaTime = 0;
	//isPlay = true;
	//pos = models.back()->Position();
	//targetPos = D3DXVECTOR3(0, 0, 10);

	animation = new AnimationClip();
	animation->SetGameModel(models.back());
}

TestBoneAnimation::~TestBoneAnimation()
{
	SAFE_DELETE(shader);

	for (GameModel * model : models)
		SAFE_DELETE(model);

	//SAFE_DELETE(sky);

	//SAFE_DELETE(renderTarget);
	//SAFE_DELETE(render2D);
	//SAFE_DELETE(postEffect);

	SAFE_DELETE(animation);
}

void TestBoneAnimation::Update()
{
	//sky->Update();
	
	for (GameModel * model : models)
		model->Update();

	//// test
	//if (isPlay) {
	//	deltaTime += Time::Delta();
	//	//pos = models.back()->Position();
	//	D3DXVECTOR3 temp;
	//	D3DXVec3Lerp(&temp, &pos, &targetPos, deltaTime / 3);
	//	models.back()->Position(temp);
	//	if (temp.z >= targetPos.z)
	//		isPlay = false;
	//}

	animation->Update();
}

void TestBoneAnimation::PreRender()
{
	//renderTarget->Set();

	//sky->Render();
	//for (GameModel * model : models)
	//	model->Render();
}

void TestBoneAnimation::Render()
{
	for (GameModel * model : models)
		model->Render();
}

void TestBoneAnimation::PostRender()
{
	//render2D->Update();
	//render2D->SRV(renderTarget->GetSRV());
	//render2D->Render();

	//postEffect->Update();
	//postEffect->SRV(renderTarget->GetSRV());
	//postEffect->Render();

	//// test
	//ImGui::Text("%f", deltaTime);
	//ImGui::Text("world Time %f", Time::Get()->Running());

	// Model 정보
	ImGui::Begin("Model Info");
	for (UINT i = 0; i<models.size(); i++)
	{
		GameModel * model = models[i];
		vector<Material*> materials = model->GetModel()->Materials();

		if (Keyboard::Get()->Press(VK_LSHIFT)
			&& Mouse::Get()->Down(0))
		{
			if (model->IsPickModelMesh(
				values->MainCamera, values->Viewport, values->Perspective,
				selectModelMesh))
			{
				selectModel = i;
				break;
			}
			else
				selectModel = -1;
		}

		if (selectModel == i)
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1),
				("Model : " + model->GetName()).c_str());
			ImGui::Text("Transform");

			D3DXVECTOR3 S, R, T;
			S = model->Scale();
			R = model->Rotation();
			T = model->Position();

			ImGui::Text("Position");
			ImGui::DragFloat3(" T X Y Z", (float*)&T, 0.1f);

			ImGui::Text("Rotation");
			ImGui::DragFloat3(" R X Y Z", (float*)&R, 0.01f);

			ImGui::Text("Scale");
			ImGui::DragFloat3(" S X Y Z", (float*)&S, 0.01f);

			model->Scale(S);
			model->Rotation(R);
			model->Position(T);
		}
	}
	ImGui::End();

	// Model의 Matrial 정보
	ImGui::Begin("Model Material Info");
	{
		if (selectModel != -1)
		{
			GameModel* model = models[selectModel];
			ImGui::TextColored(ImVec4(1, 1, 0, 1),
				(model->GetName() + " Materials").c_str());

			vector<Material*> materials = model->GetModel()->Materials();
			for (UINT i = 0; i < materials.size(); i++)
			{
				ImGui::Separator();
				if (model->GetModel()->MeshByName(selectModelMesh)->MaterialName()
					!= materials[i]->Name())
				{
					ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1),
						String::ToString(materials[i]->Name()).c_str());
				}
				else
				{
					ImGui::Text(String::ToString(materials[i]->Name()).c_str());
					ImGui::ColorEdit4("Color", (float*)materials[i]->GetDiffuse());
				}
			}
		}
	}
	ImGui::End();

	// Model의 Mesh 정보
	ImGui::Begin("Model Mesh Info");
	{
		if (selectModel != -1) {
			GameModel* model = models[selectModel];
			ImGui::TextColored(ImVec4(1, 1, 0, 1),
				(model->GetName() + " Meshes").c_str());

			vector<wstring> meshNames = model->GetMeshNames();
			for (UINT i = 0; i < meshNames.size(); i++) {
				ImGui::Separator();
				if (meshNames[i] != selectModelMesh)
					ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1),
						String::ToString(meshNames[i]).c_str());
				else
				{
					ImGui::Text(String::ToString(meshNames[i]).c_str());
					ModelBone* bone = model->GetModel()->BoneByName(selectModelMesh);
					D3DXMATRIX matWorld = bone->Local();

					D3DXVECTOR3 scale, translation;
					D3DXQUATERNION rotation;
					D3DXMatrixDecompose(&scale, &rotation, &translation,
						&matWorld);

					float pitch, yaw, roll;
					Math::QuatToPitchYawRoll(rotation, pitch, yaw, roll);

					float t[] = { translation.x, translation.y, translation.z };
					float r[] = { yaw, pitch, roll };
					float s[] = { scale.x, scale.y, scale.z };

					D3DXMATRIX S, R, T;

					ImGui::Text("Position");
					ImGui::DragFloat3(" T X Y Z", t, 0.5f);
					D3DXMatrixTranslation(&T, t[0], t[1], t[2]);

					ImGui::Text("Rotation");
					ImGui::DragFloat3(" R X Y Z", r, 0.01f);
					D3DXMatrixRotationYawPitchRoll(&R, r[0], r[1], r[2]);

					ImGui::Text("Scale");
					ImGui::DragFloat3(" S X Y Z", s, 0.01f);
					D3DXMatrixScaling(&S, s[0], s[1], s[2]);

					bone->Local(S * R * T);
				}
			}
		}
	}
	ImGui::End();

	// Animation 
	ImGui::Begin("Animation Info");
	{
		if (selectModel != -1)
		{
			GameModel* model = models[selectModel];
			vector<wstring> meshNames = model->GetMeshNames();
			bool isSelectMesh = false;
			for (UINT i = 0; i < meshNames.size(); i++) {
				if (meshNames[i] == selectModelMesh)
				{
					animation->SetMesh(selectModelMesh);
					if (ImGui::Button("Play", ImVec2(50, 20)))
						animation->Play();
					ImGui::SameLine(70);
					if (ImGui::Button("Stop", ImVec2(50,20)))
						animation->Stop();
					ImGui::SameLine(130);
					if (ImGui::Button("Add", ImVec2(50, 20)))
						animation->Add();
					ImGui::SameLine(190);
					if (ImGui::Button("Reset", ImVec2(50, 20)))
						animation->Reset();
					if (ImGui::Button("Save", ImVec2(50, 20)))
					{
						func = bind(&AnimationClip::Save, animation, placeholders::_1);
						Path::SaveFileDialog(L"", L"*.xml",
							L"./Animations", func);
					}
					ImGui::SameLine(70);
					if (ImGui::Button("Load", ImVec2(50, 20)))
					{
						func = bind(&AnimationClip::Load, animation, placeholders::_1);
						Path::OpenFileDialog(L"", L"*.*",
							L"./Animations", func);
					}

					ImGui::InputFloat("MaxTime", animation->GetMaxTime());
					ImGui::SliderFloat("Time", animation->GetCurTime(), 0, *animation->GetMaxTime());
					
					animation->PostRender();
					isSelectMesh = true;
				}
			}

			if (isSelectMesh == false)
				animation->SetMesh(L"");
		}
	}
	ImGui::End();
}