#include "stdafx.h"
#include "ExRetargeting.h"

#include "../Objects/GameAnimModel.h"
#include "../Model/ModelClip.h"

#include "../Renders/Lines/DebugDrawSphere2.h"

#include "../Viewer/Orbit.h"

ExRetargeting::ExRetargeting(ExecuteValues * values)
	:Execute(values)
	, exporter(NULL), isAnimMode(false)
	, bRepeat(true), bLockRoot(false), clipSpeed(3.0f), startTime(0)
	, isExportOption(false)
	, isExportMesh(false), isExportMaterial(false), isExportAnimation(false), isExportRead(false)
	, animationFile("")
{
	shader = new Shader(Shaders + L"Homework/Animation.hlsl");

	// test¿ë
	//ReadModel(Models + L"Kachujin/kachujin.mesh");
	//ReadAnimation(Models + L"xbot/Samba Dancing.anim");

	freedomCamera = values->MainCamera;
	orbitCamera = new Orbit();
	orbitCamera->SetOrbitDist(250.0f);

	visibleBuffer = new VisibleBuffer();
}

ExRetargeting::~ExRetargeting()
{
	for (ModelClip* clip : clips)
		SAFE_DELETE(clip);

	for (GameAnimModel* model : models)
		SAFE_DELETE(model);

	SAFE_DELETE(shader);

	SAFE_DELETE(orbitCamera);

	SAFE_DELETE(visibleBuffer);
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
	visibleBuffer->SetPSBuffer(10);
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
				if (ImGui::MenuItem("Open Fbx")) {
					func = bind(&ExRetargeting::OpenFbx, this, placeholders::_1);
					Path::OpenFileDialog(L"", Path::FbxModelFilter, Assets, func);
				}

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
					if (models.size() != 0) {
						if (clips.size() != 0) {
							for(ModelClip* clip : clips)
								SAFE_DELETE(clip);
							clips.clear();
						}
						for (GameAnimModel * model : models)
							SAFE_DELETE(model);
						models.clear();
					}
				}

				if (ImGui::MenuItem("Delete Clip")) {
					if (clips.size() != 0) {
						if (models.size() != 0)
							models.back()->Clips().clear();

						for (ModelClip* clip : clips)
							SAFE_DELETE(clip);
						clips.clear();
					}
				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (isExportOption == true) {
			ImGui::Begin("Export Options");

			ImGui::Text("Fbx Name : %s", String::ToString(file).c_str());

			ImGui::Checkbox("Export Mesh", &isExportMesh);
			ImGui::Checkbox("Export Material", &isExportMesh);
			ImGui::Checkbox("Export Animation", &isExportAnimation);
			ImGui::Checkbox("Export & Read", &isExportRead);

			ImGui::Text("Animation Name : ");
			ImGui::SameLine(130);
			string temp = String::ToString(file);
			strcpy(animationFile, temp.c_str());

			if (ImGui::InputText("", animationFile, IM_ARRAYSIZE(animationFile))) {
				temp = animationFile;
				file = String::ToWString(temp);
			}

			if (ImGui::Button("Export")) {
				if (isExportMesh)
					ExportMesh();
				if (isExportMesh)
					ExportMaterial();
				if (isExportAnimation) {
					//ExportAnimation(String::ToWString(animationFile));
					func = bind(&ExRetargeting::ExportAnimation, this, placeholders::_1);
					Path::SaveFileDialog(file + L".anim", L"Anim Files(*.anim)\0*.anim\0", Models, func);
				}
				isExportOption = false;
			}
			ImGui::SameLine(60);

			if (ImGui::Button("Cancel"))
				isExportOption = false;

			ImGui::End();
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

		ImGui::Checkbox("Visible NormalMap", (bool*)&visibleBuffer->Data.VisibleNormal);
		ImGui::Checkbox("Visible SpecularMap", (bool*)&visibleBuffer->Data.VisibleSpecular);

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

void ExRetargeting::OpenFbx(wstring path)
{
	SAFE_DELETE(exporter);

	file = Path::GetFileNameWithoutExtension(path);

	exporter = new Fbx::Exporter(path);
	isExportOption = true;
}

void ExRetargeting::ExportMaterial()
{
	exporter->ExportMaterial(Models + this->file + L"/", this->file + L".material");

	if (isExportRead) {
		models.push_back(new GameAnimModel(
			Models + file + L"/", file + L".material",
			Models + file + L"/", file + L".mesh"));
		models.back()->SetShader(shader);
		string name = String::ToString(Path::GetFileNameWithoutExtension(file));
		models.back()->Name(name);
	}
}

void ExRetargeting::ExportMesh()
{
	exporter->ExportMesh(Models + this->file + L"/", this->file + L".mesh");
}

void ExRetargeting::ExportAnimation(wstring file)
{
	wstring saveFolder = Path::GetDirectoryName(file);
	wstring fileName = Path::GetFileName(file);

	exporter->ExportAnimation(saveFolder, fileName, 0);

	if (isExportRead) {
		ReadAnimation(file);
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

