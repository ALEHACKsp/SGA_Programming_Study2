#include "stdafx.h"
#include "ExRetargeting.h"

#include "../Objects/GameAnimModel.h"
#include "../Model/ModelClip.h"

#include "../Renders/Lines/DebugDrawSphere2.h"

#include "../Viewer/Orbit.h"

ExRetargeting::ExRetargeting(ExecuteValues * values)
	:Execute(values)
	, exporter(NULL), isAnimMode(false)
	, bRepeat(true), bLockRoot(true), clipSpeed(1.0f), startTime(0)

{
	shader = new Shader(Shaders + L"029_Animation.hlsl");

	// test¿ë
	ReadModel(Models + L"Kachujin/kachujin.mesh");
	ReadAnimation(Models + L"Animations/Samba Dancing.anim");

	freedomCamera = values->MainCamera;
	orbitCamera = new Orbit();
	orbitCamera->SetOrbitDist(150.0f);
}

ExRetargeting::~ExRetargeting()
{
	for (ModelClip* clip : clips)
		SAFE_DELETE(clip);

	for (GameAnimModel* model : models)
		SAFE_DELETE(model);

	SAFE_DELETE(shader);

	SAFE_DELETE(orbitCamera);
}

void ExRetargeting::Update()
{
	if (models.size() != 0) {
		if (Mouse::Get()->Down(0) && Keyboard::Get()->Press(VK_LCONTROL))
		{
			D3DXVECTOR3 start, dir;
			values->MainCamera->Position(&start);
			dir = values->MainCamera->Direction(values->Viewport, values->Perspective);
			models.back()->CheckPickMesh(start, dir);
		}

		models.back()->Update();

		int selectBone = models.back()->SelectBoneIndex();
		if (selectBone == -1) 
			selectBone = 0;
		D3DXVECTOR3 targetPos = models.back()->BonePosition(selectBone);

		orbitCamera->SetTarget(targetPos);
	}
}

void ExRetargeting::PreRender()
{

}

void ExRetargeting::Render()
{
	if (models.size() != 0) models.back()->Render();
}

void ExRetargeting::PostRender()
{
	// File
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Read Model")) {
					func = bind(&ExRetargeting::ReadModel, this, placeholders::_1);
					Path::OpenFileDialog(L"", L"Mesh Files(*.mesh)\0*.mesh\0", Models, func);
				}
				if (ImGui::MenuItem("Read Animation")) {
					func = bind(&ExRetargeting::ReadAnimation, this, placeholders::_1);
					Path::OpenFileDialog(L"", L"Anim Files(*.anim)\0*.anim\0", Models, func);
				}

				if (ImGui::MenuItem("Save Animation")) {
					func = bind(&ExRetargeting::SaveAnimation, this, placeholders::_1);
					Path::SaveFileDialog(L"", L"Anim Files(*.anim)\0*.anim\0", Models, func);
				}

				if (ImGui::MenuItem("Delete Model")) {
					if (models.size() != 0)
						models.pop_back();
				}

				if (ImGui::MenuItem("Delete Clip")) {
					if (clips.size() != 0)
						clips.pop_back();
				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	// Mode
	{
		ImGui::Begin("Mode");

		if (ImGui::Checkbox("Animation Check", &isAnimMode)) {
			if (isAnimMode == false) {
				//values->MainCamera->Position(110.0f, 90.0f, -300.0f);
				values->MainCamera = freedomCamera;
				models.back()->Stop();
				models.back()->Clips().clear();
			}
			else
				//values->MainCamera->Position(12.5f, 90.0f, -300.0f);
				values->MainCamera = orbitCamera;
		}

		ImGui::End();
	}

	// Model or Info
	{
		if (isAnimMode == false) {
			ImGui::Begin("Model");

			if (models.size() != 0) {
				models.back()->PostRender();
				ImGui::Separator();
				models.back()->BoneRender();
			}

			ImGui::End();
		}
		else {
			ImGui::Begin("Info");

			if (models.size() != 0)
				ImGui::TextColored(ImVec4(255, 255, 0, 255), 
					("Model " + models.back()->Name()).c_str());
			if (clips.size() != 0) {
				ImGui::TextColored(ImVec4(0, 255, 0, 255),
					("Animation " + String::ToString(clips.back()->Name())).c_str());

				if (models.size() != 0 && models.back()->Clips().size() == 0)
					models.back()->AddClip(clips.back());
			}

			if (models.size() != 0) 
				models.back()->PostRender();

			ImGui::End();
		}
	}

	// Animation 
	{
		if (isAnimMode == false) {
			ImGui::Begin("Animation");

			if (clips.size() != 0) {
				clips.back()->PostRender(models.back()->SelectBoneName());
			}

			ImGui::End();
		}
		else {
			ImGui::Begin("Animation Configure");

			if (models.size() != 0) {
				GameAnimModel* model = models.back();
				if (clips.size() != 0)
				{
					ModelClip * clip = model->Clips().back();

					string name = String::ToString(clip->Name());
					ImGui::Text(("Animation : " + name).c_str());

					if (ImGui::Checkbox("LockRoot", &bLockRoot))
						clip->LockRoot(bLockRoot);
					if (ImGui::Checkbox("Repeat", &bRepeat))
						clip->Repeat(bRepeat);
					if (ImGui::SliderFloat("Speed", &clipSpeed, 0, 100))
						clip->Speed(clipSpeed);

					ImGui::Separator();

					if (ImGui::Button("Play")) {
						models.back()->Play(0, bRepeat, 0, clipSpeed, startTime);
						models.back()->LockRoot(0, bLockRoot);
					}

					ImGui::SameLine(60);
					if (models.back()->IsPause() == false) {
						if (ImGui::Button("Pause"))
							models.back()->Pause();
					}
					else {
						if (ImGui::Button("Resume"))
							models.back()->Resume();
					}

					ImGui::SameLine(120);
					if (ImGui::Button("Stop")) 
						models.back()->Stop();

					ImGui::Separator();

					float playTime = clip->PlayTime();
					ImGui::DragFloat("Play Time", &playTime, clip->Speed(), 0, clip->Duration());
					clip->PlayTime(playTime);
					int frame = (playTime / clip->Duration()) * clip->FrameCount();
					if (ImGui::SliderInt("Frame", &frame, 0, clip->FrameCount() - 1)) {
						int boneIndex = models.back()->SelectBoneIndex();
						clip->GetKeyframeMapTime(
							models.back()->GetModel()->BoneByIndex(boneIndex)->Name(), frame, &playTime);
						clip->PlayTime(playTime);
					}
				}
			}

			ImGui::End();
		}
	}
}

void ExRetargeting::ReadModel(wstring file)
{
	wstring path = Path::GetDirectoryName(file);
	wstring fileName = Path::GetFileNameWithoutExtension(file);

	models.push_back(new GameAnimModel(
		path, fileName + L".material",
		path, fileName + L".mesh"));
	models.back()->SetShader(shader);
	models.back()->Name(String::ToString(fileName));
}

void ExRetargeting::ReadAnimation(wstring file)
{
	clips.push_back(new ModelClip(file));

	wstring name = Path::GetFileNameWithoutExtension(file);
	clips.back()->Name(name);
}

void ExRetargeting::SaveAnimation(wstring file)
{
	if(clips.size() != 0)
		clips.back()->Save(file);
}

