#include "stdafx.h"
#include "AnimationTool.h"
#include "./Objects/GameModel.h"
#include "./Utilities/BinaryFile.h"
#include "./Objects/GameAnimModel.h"
#include "./Model/ModelClip.h"
#include "./Draw/MeshSphere.h"
#include "./Objects/PickingObject.h"

AnimationTool::AnimationTool(ExecuteValues* values)
	: selectMaterial(-1), selectMesh(-1), selectAnimation(-1), values(values)
	, bLockRoot(true), bRepeat(true), bShowBones(false), bRetargeting(false), boneNameBuf(L""), interpolateFrame(3)
	, selectModel(NULL), exporter(NULL)
{
	//sphere = new MeshSphere();
	//sphere->SetDiffuse(1, 0, 0);
	//sphere->Scale(15.0f, 15.0f, 15.0f);
	LoadDataList();

	pickingObj = new PickingObject(values);
}

AnimationTool::~AnimationTool()
{
	SAFE_DELETE(pickingObj);
	SAFE_DELETE(selectModel);
	SAFE_DELETE(sphere);
}

void AnimationTool::Update()
{
	if (selectModel != NULL)
		selectModel->Update();

	if (Mouse::Get()->Up(0))
		pickingObj->PickingBone();
}

void AnimationTool::Render()
{
	if (selectModel != NULL)
	{
		selectModel->Render();
		RenderBone();
	}

	ImGui::Begin("Animation Editor", 0, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load Fbx File"))
			{
				Path::OpenFileDialog(L"", L"fbx Files(*.fbx)\0*.fbx\0", Assets, bind(&AnimationTool::LoadFbxFile, this, placeholders::_1));
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
	if (ImGui::TreeNode("Materials"))
	{
		for (UINT i = 0; i < materials.size(); i++)
		{
			char buf[128];
			sprintf(buf, "%s", materials[i].c_str());
			if (ImGui::Selectable(buf, selectMaterial == i))
			{
				selectMaterial = i;
				if (selectMaterial > -1 && selectMesh > -1)
				{
					SelectModel();
				}
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Meshes"))
	{
		for (UINT i = 0; i < meshes.size(); i++)
		{
			char buf[128];
			sprintf(buf, "%s", meshes[i].c_str());
			if (ImGui::Selectable(buf, selectMesh == i))
			{
				selectMesh = i;
				if (selectMaterial > -1 && selectMesh > -1)
				{
					SelectModel();
				}
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Animations"))
	{
		for (UINT i = 0; i < animations.size(); i++)
		{
			char buf[128];
			sprintf(buf, "%s", Path::GetFileNameWithoutExtension(animations[i]).c_str());
			if (ImGui::Selectable(buf, selectAnimation == i))
			{
				selectAnimation = i;
				if (selectMaterial > -1 && selectMesh > -1)
				{
					SelectModel();
					selectModel->AddClip(Models + String::ToWString(materials[selectMaterial]) + L"/" + String::ToWString(animations[selectAnimation]) + L".anim");
					selectModel->LockRoot(0, bLockRoot);
					selectModel->Repeat(0, bRepeat);
					selectModel->Speed(0, 15.0f);

					selectModel->Play(0, true, 0.0f, 20.0f);
				}
			}
		}
		ImGui::TreePop();
	}
	ImGui::End();

	if (selectMaterial > -1 && selectMesh > -1 && selectAnimation > -1)
	{
		RenderAnimationEditor();
	}

	if (exporter != NULL)
		RenderFbxCheckBox();
}

void AnimationTool::PostRender()
{
	//if (pickingObj->GetPickedPart() != NULL)
	//{
	//	DirectWrite::RenderText(
	//		pickingObj->GetPickedPart()->Name()
	//		, values->MainCamera->ScreenPosition(values->Viewport, values->Perspective, sphere->World(), 500, 500)
	//	);
	//}
}

void AnimationTool::LoadDataList()
{
	materials.clear();
	meshes.clear();
	animations.clear();
	Path::GetFiles(&materials, String::ToString(Models), "*.material", true);
	Path::GetFiles(&meshes, String::ToString(Models), "*.mesh", true);
	Path::GetFiles(&animations, String::ToString(Models), "*.anim", true);

	for (UINT i = 0; i < materials.size(); i++)
	{
		materials[i] = Path::GetFileNameWithoutExtension(materials[i]);
	}
	for (UINT i = 0; i < meshes.size(); i++)
	{
		meshes[i] = Path::GetFileNameWithoutExtension(meshes[i]);
	}
	for (UINT i = 0; i < animations.size(); i++)
	{
		animations[i] = Path::GetFileNameWithoutExtension(animations[i]);
	}
}

void AnimationTool::LoadFbxFile(wstring fileName)
{
	exporter = new Fbx::Exporter(fileName);
	this->fileName = Path::GetFileNameWithoutExtension(fileName);

	bExport[0] = exporter->GetScene()->GetMaterialCount() > 0 ? true : false;
	bExport[1] = exporter->GetScene()->GetRootNode()->GetNodeAttribute() != NULL ? true : false;
	int animCount = exporter->GetAnimCount();
	bExport[2] = animCount >= 0 ? true : false;
	for (int i = 0; i < animCount; ++i)
	{
		animNames.push_back(exporter->GetAnimName(i));
		exportNames.assign(animNames.begin(), animNames.end());
	}
}

void AnimationTool::RenderFbxCheckBox()
{
	ImGui::Begin("Fbx Exporter");
	if (bExport[0] == true)
		ImGui::Checkbox("Material", &bExportCheck[0]);
	if (bExport[1] == true)
		ImGui::Checkbox("Mesh", &bExportCheck[1]);
	if (bExport[2] == true)
	{
		ImGui::Text("Animation");
		for (UINT i = 0; i < exportNames.size(); ++i)
		{
			ImGui::NewLine(); ImGui::SameLine(20);
			char label[32];
			sprintf(label, "%d", i);
			ImGui::Checkbox(label , &bExportCheck[i + 2]); ImGui::SameLine(50);
			char buf[64] = "";
			for (UINT j = 0; j < exportNames[i].length(); j++)
			{
				if (j == 64) break;
				buf[j] = exportNames[i].c_str()[j];
			}
			sprintf(label, "FileName-%d", i);
			ImGui::InputText(label, buf, 64);
			exportNames[i] = buf;
		}
	}
	ImGui::BeginGroup();
	if (ImGui::Button("Export"))
		ExportFbxFile();
	ImGui::SameLine(55);
	if (ImGui::Button("Cancel"))
		SAFE_DELETE(exporter);
	ImGui::EndGroup();
	ImGui::End();
}

void AnimationTool::ExportFbxFile()
{
	//material
	if (bExport[0] == true && bExportCheck[0] == true)
	{
		exporter->ExportMaterial(Models + fileName + L"/", fileName + L".material");
	}
	//mesh
	if (bExport[1] == true && bExportCheck[1] == true)
	{
		exporter->ExportMesh(Models + fileName + L"/", fileName + L".mesh");
	}
	//animation
	if (bExport[2] == true)
	{
		for (UINT i = 0; i < animNames.size(); ++i)
		{
			if (bExportCheck[i + 2])
			{
				wstring temp = String::ToWString(exportNames[i].c_str());
				exporter->ExportAnimation(Models + L"kachujin" + L"/", temp + L".anim", i);
			}
		}
	}
	SAFE_DELETE(exporter);
	LoadDataList();
}

void AnimationTool::SaveAnimation(wstring fileName)
{
}

void AnimationTool::SelectModel()
{
	SAFE_DELETE(selectModel);
	selectModel = new GameAnimModel(Models + String::ToWString(materials[selectMaterial]) + L"/"
		, String::ToWString(materials[selectMaterial]) + L".material"
		, Models + String::ToWString(meshes[selectMesh]) + L"/"
		, String::ToWString(meshes[selectMesh]) + L".mesh");
	if (selectAnimation > -1)
		selectModel->SetShader(new Shader(Shaders + L"035_Animation.hlsl"));
	else
		selectModel->SetShader(new Shader(Shaders + L"034_Normalmap.hlsl"));
	pickingObj->SetPickedPart(NULL);
	pickingObj->SetPickedObj(selectModel);
}

void AnimationTool::RenderBone()
{
	if (pickingObj->GetPickedPart() != NULL)
	{
		vector<D3DXMATRIX>* boneTransforms = selectModel->GetBoneTransforms();
		D3DXMATRIX matrix = pickingObj->GetPickedPart()->Global() * (*boneTransforms)[pickingObj->GetPickedPart()->Index()];
		D3DXVECTOR3 translation, scale;
		D3DXQUATERNION rotation;
		D3DXMatrixDecompose(&scale, &rotation, &translation, &matrix);
		//sphere->Scale(scale * 15.0f);
		//sphere->Position(translation);
		//sphere->Rotation(Math::GetRotation(rotation));
		//sphere->Update();
		//sphere->Render();

	}
}

void AnimationTool::RenderAnimationEditor()
{
	ImGui::Begin("Animation");
	if (ImGui::Checkbox("LockRoot", &bLockRoot))
		selectModel->LockRoot(0, bLockRoot);
	if (ImGui::Checkbox("Repeat", &bRepeat))
		selectModel->Repeat(0, bRepeat);
	ImGui::Separator();
	ImGui::SliderFloat("TimeLine", selectModel->GetClip(0)->GetPlayTime(), 0, selectModel->GetClip(0)->GetDuration());
	ImGui::BeginGroup();
	{
		if (ImGui::Button("Play")) selectModel->Play(); ImGui::SameLine(50);
		if (ImGui::Button("Stop")) selectModel->Stop();
	}
	ImGui::EndGroup();

	if (pickingObj->GetPickedPart() != NULL)
	{
		ImGui::BeginGroup();
		D3DXVECTOR3 scale, translation, outRotation;
		D3DXQUATERNION rotation;
		D3DXMatrixDecompose(&scale, &rotation, &translation, &selectModel->GetClip(0)->GetKeyframeAnimMatrix(pickingObj->GetPickedPart()));
		outRotation = Math::GetRotation(rotation);
		bool b = false;
		ImGui::Separator();
		ImGui::Text(String::ToString(pickingObj->GetPickedPart()->Name()).c_str());
		if (ImGui::DragFloat3("Scale", (float*)&scale)) b = true;
		if (ImGui::DragFloat3("rotation", (float*)&outRotation)) b = true;
		if (ImGui::DragFloat3("translation", (float*)&translation)) b = true;
		if (b == true)
		{
			D3DXMATRIX S, R, T;
			D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
			D3DXMatrixRotationYawPitchRoll(&R, outRotation.y, outRotation.x, outRotation.z);
			D3DXMatrixTranslation(&T, translation.x, translation.y, translation.z);
			float time = *selectModel->GetClip(0)->GetPlayTime();
			selectModel->GetClip(0)->SetKeyframeMatrix(pickingObj->GetPickedPart(), S * R * T, interpolateFrame);
		}
		ImGui::DragInt("InterpolateFrameCount", (int*)&interpolateFrame, 0, 10);
		ImGui::EndGroup();
		ImGui::Separator();

		if (bRetargeting == true)
		{
			if (ImGui::Button("Retargeting End"))
				bRetargeting = false;
			RenderAnimationRetargeting();
		}
		else
		{
			if (ImGui::Button("Retargeting"))
				bRetargeting = true;
		}
	}
	ImGui::End();
}

void AnimationTool::RenderAnimationRetargeting()
{
	ImGui::Begin("Retargeting Bones");
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("Child1", ImVec2(ImGui::GetWindowSize().x / 2, -ImGui::GetFrameHeightWithSpacing()), true);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("MeshBones"))
			{
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		//본 출력
		RenderBoneNames(selectModel->GetModel()->BoneByIndex(0));
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::BeginChild("Child2", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("AnimationBones"))
			{
		
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		//본 출력
		unordered_map<wstring, ModelKeyframe *>::iterator it;
		//for (it = selectModel->GetClip(0)->GetKeyframeMap()->begin(); it != selectModel->GetClip(0)->GetKeyframeMap()->end(); ++it)
		//{
		//	ImGui::Selectable(String::ToString(it->first).c_str());
		//	if (ImGui::BeginDragDropTarget())
		//	{
		//		if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload("BoneName"))
		//		{
		//			IM_ASSERT(payLoad->DataSize == sizeof(ModelBone));
		//			ModelBone* bone = (ModelBone*)payLoad->Data;
		//			pair<wstring, ModelKeyframe *> pair;
		//			pair.first = bone->Name();
		//			pair.second = it->second;
		//			it = selectModel->GetClip(0)->GetKeyframeMap()->erase(it);
		//			selectModel->GetClip(0)->GetKeyframeMap()->insert(pair);
		//		}
		//		ImGui::EndDragDropTarget();
		//	}
		//}

		ImGui::EndChild();
		ImGui::EndGroup();

		ImGui::Text(String::ToString(boneNameBuf).c_str());

		ImGui::PopStyleVar();
	}
	ImGui::End();
}

void AnimationTool::RenderBoneNames(ModelBone * bone)
{
	if (bone->ChildCount() > 0)
	{
		boneNameBuf = bone->Name();
		if (ImGui::TreeNode(String::ToString(boneNameBuf).c_str()))
		{
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("BoneName", (void *)bone, sizeof(ModelBone));
				ImGui::Text(String::ToString(bone->Name()).c_str());
				ImGui::EndDragDropSource();
			}
			for (UINT i = 0; i < bone->ChildCount(); i++)
				RenderBoneNames(bone->GetChilds()[i]);
			ImGui::TreePop();
		}
	}
	else
	{
		if (ImGui::Selectable(String::ToString(bone->Name()).c_str()))
		{
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("BoneName", (void *)bone, sizeof(ModelBone));
				ImGui::EndDragDropSource();
			}
		}
	}
}
