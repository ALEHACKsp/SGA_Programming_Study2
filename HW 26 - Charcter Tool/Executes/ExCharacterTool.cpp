#include "stdafx.h"
#include "ExCharacterTool.h"

#include "../Objects/GameAnimModel.h"
#include "../Model/ModelClip.h"

#include "../Renders/Lines/DebugDrawSphere2.h"

ExCharacterTool::ExCharacterTool(ExecuteValues * values)
	:Execute(values)
	, clipSelect(false), exporter(NULL)
	, isExportOption(false)
	, isExportMesh(false), isExportMaterial(false), isExportAnimation(false), isExportRead(false)
	, animationFile("")
	, imGuiWindowNoMove(false), showAllBoneTree(true)
	, selectModel(0), selectClip(0), selectMode(0), selectModelClip(0)
	, bRepeat(true), bLockRoot(true), clipSpeed(1.0f), startTime(0)
{
	shader = new Shader(Shaders + L"029_Animation.hlsl");

	// test¿ë
	//ReadModel(Models + L"xbot/xbot.mesh");
	//ReadAnimation(Models + L"Animations/Samba Dancing.anim");
	//ReadAnimation(Models + L"Animations/Praying.anim");
	//for (int i = 0; i < clips.size(); i++)
	//	models.back()->AddClip(clips[i]);
}

ExCharacterTool::~ExCharacterTool()
{
	for (ModelClip* clip : clips)
		SAFE_DELETE(clip);

	for (GameAnimModel* model : models)
		SAFE_DELETE(model);

	SAFE_DELETE(shader);
}

void ExCharacterTool::Update()
{
	if (selectModel < models.size()) {

		if (Mouse::Get()->Down(0) && Keyboard::Get()->Press(VK_LCONTROL))
		{
			D3DXVECTOR3 start, dir;
			values->MainCamera->Position(&start);
			dir = values->MainCamera->Direction(values->Viewport, values->Perspective);
			models[selectModel]->CheckPickMesh(start, dir);

		}
		models[selectModel]->Update();
	}
}

void ExCharacterTool::PreRender()
{

}

void ExCharacterTool::Render()
{
	if (selectModel < models.size())
		models[selectModel]->Render();
}

void ExCharacterTool::PostRender()
{
	ImGui::Checkbox("Window No Move", &imGuiWindowNoMove);

	// File
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open Fbx")) {
					func = bind(&ExCharacterTool::OpenFbx, this, placeholders::_1);
					Path::OpenFileDialog(L"", Path::FbxModelFilter, Assets, func);
				}
				if (ImGui::MenuItem("Read Model")) {
					func = bind(&ExCharacterTool::ReadModel, this, placeholders::_1);
					Path::OpenFileDialog(L"", L"Mesh Files(*.mesh)\0*.mesh\0", Models, func);
				}
				if (ImGui::MenuItem("Read Animation")) {
					func = bind(&ExCharacterTool::ReadAnimation, this, placeholders::_1);
					Path::OpenFileDialog(L"", L"Anim Files(*.anim)\0*.anim\0", Models, func);
				}

				if (ImGui::MenuItem("Save Animation")) {
					func = bind(&ExCharacterTool::SaveAnimation, this, placeholders::_1);
					Path::SaveFileDialog(L"", L"Anim Files(*.anim)\0*.anim\0", Models, func);
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
			for (int i = 0; i < temp.size(); i++)
				animationFile[i] = temp[i];
			ImGui::InputText("", animationFile, IM_ARRAYSIZE(animationFile));

			if (ImGui::Button("Export")) {
				if (isExportMesh) 
					ExportMesh();
				if (isExportMesh)
					ExportMaterial();
				if (isExportAnimation) 
					ExportAnimation(String::ToWString(animationFile));
				isExportOption = false;
			}
			ImGui::SameLine(60);

			if (ImGui::Button("Cancel"))
				isExportOption = false;

			ImGui::End();
		}
	}

	ImVec2 itemMin;
	ImVec2 itemMax;

	// Model & Clip List
	{
		ImGuiWindowFlags flag = imGuiWindowNoMove == true ? ImGuiWindowFlags_NoMove : 0;
		ImGui::Begin("List", 0, flag);

		ImGui::TextColored(ImVec4(255,255,0,255), "Models");
		for (int i = 0; i < models.size(); i++) {
			ImGui::InvisibleButton("Dummy", ImVec2(ImGui::GetWindowSize().x - 30, 20));
			itemMin = ImGui::GetItemRectMin();
			itemMax = ImGui::GetItemRectMax();
			if (ImGui::IsItemClicked()) {
				selectModel = i;
				if (models[selectModel]->Clips().size() - 1 >= selectModelClip)
					selectModelClip = models[selectModel]->Clips().size() - 1;
			}
			
			ImGui::SameLine(ImGui::GetWindowSize().x - 20);
			if (ImGui::ColorButton(("delete Model " + to_string(i)).c_str(), 
				ImVec4(255,0,0,255), ImGuiColorEditFlags_NoTooltip, ImVec2(20, 20))) {
				SAFE_DELETE(models[i]);
				models.erase(models.begin() + i);
				if (selectModel >= i)
					selectModel--;
				if (selectModel < 0 && models.size() != 0)
					selectModel = 0;
				continue;
			}

			if (selectModel == i)
				ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 255, 0, 128));

			ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 255, 255), models[i]->Name().c_str());

			ImGui::Indent();
			vector<class ModelClip *> clips = models[i]->Clips();
			for (int j = 0; j < clips.size(); j++) {
				ImGui::InvisibleButton("Dummy", ImVec2(ImGui::GetWindowSize().x - 100, 20));
				itemMin = ImGui::GetItemRectMin();
				itemMax = ImGui::GetItemRectMax();
				if (ImGui::IsItemClicked()) {
					selectModelClip = j;
					models[selectModel]->SelectClip(selectModelClip);
				}

				ImGui::SameLine(ImGui::GetWindowSize().x - 20);
				if (ImGui::ColorButton(("delete Model Clip " + to_string(j)).c_str(),
					ImVec4(255, 0, 0, 255), ImGuiColorEditFlags_NoTooltip, ImVec2(20, 20))) {
					models[i]->DeleteClip(j);

					if (selectModelClip >= j)
						selectModelClip--;
					if (selectModelClip < 0 && clips.size() != 0)
						selectModelClip = 0;

					continue;
				}

				if (j == selectModelClip)
					ImGui::GetWindowDrawList()->AddRectFilled(
						ImVec2(itemMin.x - 20, itemMin.y), 
						ImVec2(itemMin.x, itemMax.y), IM_COL32(0, 0, 255, 128));

				string name = String::ToString(clips[j]->Name());
				ImGui::GetWindowDrawList()->AddText(
					ImVec2(itemMin.x + 20, itemMin.y), IM_COL32(0, 255, 0, 255), name.c_str());
			}
			ImGui::Unindent();
		}
		ImGui::Separator();

		ImGui::TextColored(ImVec4(0,255,0,255), "Clips");
		for (int i = 0; i < clips.size(); i++) {
			ImGui::InvisibleButton("Dummy", ImVec2(ImGui::GetWindowSize().x - 30, 20));
			itemMin = ImGui::GetItemRectMin();
			itemMax = ImGui::GetItemRectMax();

			if (ImGui::IsItemClicked()) {
				selectClip = i;
				
				clipSelect = true;
			}

			ImGui::SameLine(ImGui::GetWindowSize().x - 20);
			if (ImGui::ColorButton(("delete Clip " + to_string(i)).c_str(),
				ImVec4(255, 0, 0, 255), ImGuiColorEditFlags_NoTooltip, ImVec2(20, 20))) {
				for (int j = 0; j < models.size(); j++) {
					models[j]->DeleteClip(clips[i]->Name());
				}
				SAFE_DELETE(clips[i]);
				clips.erase(clips.begin() + i);
				if (selectClip >= i)
					selectClip--;
				if (selectClip < 0 && clips.size() != 0)
					selectClip = 0;
				continue;
			}

			if (selectClip == i)
				ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(0, 255, 0, 128));

			ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 255, 255), 
				String::ToString(clips[i]->Name()).c_str());
		}

		if (ImGui::IsMouseReleased(0) && !ImGui::IsWindowHovered() &&
			clipSelect) {
			clipSelect = false;
			if(selectClip >= 0)
			models[selectModel]->AddClip(clips[selectClip]);
		}

		ImGui::End();
	}

	// Model Info
	{
		ImGui::Begin("Hierarchy");
		
	
		if (selectMode == 0) {
			ImGui::Checkbox("Show All Model Bone", &showAllBoneTree);
			ImGui::Separator();
		}
		else
			showAllBoneTree = false;

		if (models.size() != 0 && selectModel >= 0) {
			if (selectMode == 1) {
				vector<ModelClip*> clips = models[selectModel]->Clips();
				if (clips.size() != 0 && selectModelClip >= 0) {
					ModelClip* clip = clips[selectModelClip];

					float playTime = clip->PlayTime();
					ImGui::DragFloat("Play Time", &playTime, clip->Speed(), 0, clip->Duration());
					clip->PlayTime(playTime);
					int frame = (playTime / clip->Duration()) * clip->FrameCount();
					if (ImGui::SliderInt("Frame", &frame, 0, clip->FrameCount() - 1)) {
						int boneIndex = models[selectModel]->SelectBoneIndex();
						clip->GetKeyframeMapTime(
							models[selectModel]->GetModel()->BoneByIndex(boneIndex)->Name(), frame, &playTime);
						clip->PlayTime(playTime);
					}
				}
				ImGui::Separator();
			}

			if (showAllBoneTree == false)
				models[selectModel]->TreeRender(true);
			else {
				for (int i = 0; i < models.size(); i++) {
					models[i]->TreeRender(selectModel == i);
				}
			}
		}
		ImGui::End();

		ImGui::Begin("Inspector");

		if (models.size() != 0 && selectModel >= 0) {
			models[selectModel]->SetAnimMode(selectMode == 1);
			models[selectModel]->PostRender();
		}

		ImGui::End();
	}

	// Mode
	{
		ImGui::Begin("Mode");

		ImGui::RadioButton("Bone", &selectMode, 0);
		ImGui::SameLine(60);
		ImGui::RadioButton("Animation", &selectMode, 1);

		if (selectMode == 0) {
			if (models.size() != 0 && selectModel >= 0) {
				if (models[selectModel]->Clips().size() != 0) {
					models[selectModel]->Stop();
				}
			}
		}

		ImGui::End();
	}

	// Animation Info
	{
		if (selectMode == 1) {
			ImGui::Begin("Animation Configure");

			if (models.size() != 0 && selectModel >= 0) {
				GameAnimModel* model = models[selectModel];
				if (model->Clips().size() != 0 && selectModelClip >= 0)
				{
					ModelClip * clip = model->Clips()[selectModelClip];

					string name = String::ToString(clip->Name());
					ImGui::Text(("Name : " + name).c_str());

					if(ImGui::Checkbox("LockRoot", &bLockRoot))
						clip->LockRoot(bLockRoot);
					if(ImGui::Checkbox("Repeat", &bRepeat))
						clip->Repeat(bRepeat);
					if(ImGui::SliderFloat("Speed", &clipSpeed, 0, 100))
						clip->Speed(clipSpeed);
				}
			}

			ImGui::End();
		}
	}

	// Animation Player
	{
		if (selectMode == 1) {
			ImGui::Begin("Player");

			if (ImGui::Button("Play")) {
				if (models.size() != 0 && selectModel >= 0)
				{
					if (models[selectModel]->Clips().size() != 0
						&& selectModelClip >= 0) {
						models[selectModel]->Play(selectModelClip, bRepeat, 0, clipSpeed, startTime);
						models[selectModel]->LockRoot(selectModelClip, bLockRoot);
					}
				}
			}
			ImGui::SameLine(60);
			if (models.size() != 0 && selectModel >= 0)
				if (models[selectModel]->Clips().size() != 0) {
					if (models[selectModel]->IsPause() == false) {
						if (ImGui::Button("Pause"))
							models[selectModel]->Pause();
					}
					else {
						if (ImGui::Button("Resume"))
							models[selectModel]->Resume();
					}
				}
			ImGui::SameLine(120);
			if (ImGui::Button("Stop")) {
				if (models.size() != 0 && selectModel >= 0)
					if (models[selectModel]->Clips().size() != 0)
						models[selectModel]->Stop();
			}

			ImGui::End();
		}
	}
}

void ExCharacterTool::OpenFbx(wstring path)
{
	SAFE_DELETE(exporter);

	file = Path::GetFileNameWithoutExtension(path);

	exporter = new Fbx::Exporter(path);
	isExportOption = true;
}

void ExCharacterTool::ExportMaterial()
{
	exporter->ExportMaterial(Models + this->file + L"/", this->file + L".material");

	if (isExportRead) {
		models.push_back(new GameAnimModel(
			Models + file + L"/", file + L".material",
			Models + file + L"/", file + L".mesh"));
		models.back()->SetShader(shader);
		string name = String::ToString(Path::GetFileNameWithoutExtension(file));
		models.back()->Name(name);
		//selectModel = models.size() - 1;
	}
}

void ExCharacterTool::ExportMesh()
{
	exporter->ExportMesh(Models + this->file + L"/", this->file + L".mesh");
}

void ExCharacterTool::ExportAnimation(wstring file)
{
	exporter->ExportAnimation(Models + L"Animations/", this->file + L".anim", 0);

	if (isExportRead) {
		ReadAnimation(Models + L"Animations/" + file + L".anim");
	}
}

void ExCharacterTool::ReadModel(wstring file)
{
	wstring path = Path::GetDirectoryName(file);
	wstring fileName = Path::GetFileNameWithoutExtension(file);

	models.push_back(new GameAnimModel(
		path, fileName + L".material",
		path, fileName + L".mesh"));
	models.back()->SetShader(shader);
	models.back()->Name(String::ToString(fileName));
	//models.back()->RetargetBone();
	//selectModel = models.size() - 1;
}

void ExCharacterTool::ReadAnimation(wstring file)
{
	clips.push_back(new ModelClip(file));

	wstring name = Path::GetFileNameWithoutExtension(file);
	clips.back()->Name(name);
	//models.back()->AddClip(clips.back());
	//models.back()->RetargetClip();
	//models.back()->Play(0, true, 0, 20.0f);
}

void ExCharacterTool::SaveAnimation(wstring file)
{
	if (models.size() != 0 && selectModel >= 0) {
		if (models[selectModel]->Clips().size() != 0 && selectModelClip >= 0) {
			models[selectModel]->Clips()[selectModelClip]->Save(file, models[selectModel]->GetModel());
		}
	}
}

