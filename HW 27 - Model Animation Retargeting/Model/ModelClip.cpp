#include "stdafx.h"
#include "ModelClip.h"

#include "../Utilities/BinaryFile.h"
#include "../Model/ModelKeyframe.h"

ModelClip::ModelClip(wstring file)
	: bRepeat(false), speed(1.0f), playTime(0.0f), bLockRoot(false)
{
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	name = String::ToWString(r->String());
	duration = r->Float();
	frameRate = r->Float();
	frameCount = r->UInt();

	keyframesCount = r->UInt();
	for (UINT i = 0; i < keyframesCount; i++)
	{
		ModelKeyframe* keyframe = new ModelKeyframe();
		keyframe->boneName = String::ToWString(r->String());
		keyList.push_back(keyframe->boneName);

		keyframe->duration = duration;
		keyframe->frameCount = frameCount;
		keyframe->frameRate = frameRate;

		UINT size = r->UInt();
		if (size > 0)
		{
			keyframe->transforms.assign(size, ModelKeyframe::Transform());

			void* ptr = (void *)&keyframe->transforms[0];
			r->Byte(&ptr, sizeof(ModelKeyframe::Transform) * size);
		}

		keyframeMap.insert(Pair(keyframe->boneName, keyframe));
	}

	r->Close();
	SAFE_DELETE(r);

	sort(keyList.begin(), keyList.end());
}

ModelClip::~ModelClip()
{
	for (Pair keyframe : keyframeMap)
		SAFE_DELETE(keyframe.second);
}

void ModelClip::Reset()
{
	bRepeat = false;
	speed = 1.0f;
	playTime = 0.0f;
}

D3DXMATRIX ModelClip::GetKeyframeMatrix(ModelBone * bone, float time)
{
	wstring boneName = bone->Name();

	unordered_map<wstring, ModelKeyframe*>::iterator it;
	if ((it = keyframeMap.find(boneName)) == keyframeMap.end())
	{
		D3DXMATRIX temp;
		D3DXMatrixIdentity(&temp);

		return temp;
	}

	ModelKeyframe* keyframe = keyframeMap.at(boneName);

	playTime += speed * time; // time = deltaTime
	if (bRepeat == true)
	{
		if (playTime >= duration)
		{
			// 나눈 나머지 연산인데 float이라 빼는 식으로 하는거
			while (playTime - duration >= 0)
				playTime -= duration;
		}
	}
	else
	{
		if (playTime >= duration)
			playTime = duration;
	}

	D3DXMATRIX invGlobal = bone->Global();
	D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);
	
	D3DXMATRIX animation = keyframe->GetInterpolatedMatrix(playTime, bRepeat);

	D3DXMATRIX parent;
	int parentIndex = bone->ParentIndex();
	// root는 pass
	if (parentIndex < 0)
	{
		if (bLockRoot == true)
			//D3DXMatrixIdentity(&parent);
			parent = bone->Global();
		else
			parent = animation;
	}
	else
	{
		parent = animation * bone->Parent()->Global();
	}

	return invGlobal * parent;
	//return parent;
}

void ModelClip::UpdateKeyframe(ModelBone * bone, float time)
{
	D3DXMATRIX animation = GetKeyframeMatrix(bone, time);


	bone->Local(animation);
}

void ModelClip::GetKeyframeMapTransform(wstring boneName, int frame, OUT D3DXVECTOR3 * S, OUT D3DXQUATERNION * R, OUT D3DXVECTOR3 * T)
{
	keyframeMap[boneName]->GetTransform(frame, S, R, T);
}

void ModelClip::SetKeyframeMapTransform(wstring boneName, int frame, D3DXVECTOR3& S, D3DXQUATERNION& R, D3DXVECTOR3& T)
{
	keyframeMap[boneName]->SetTransform(frame, S, R, T);
}

void ModelClip::GetKeyframeMapTime(wstring boneName, int frame, OUT float * time)
{
	keyframeMap[boneName]->GetTime(frame, time);
}

void ModelClip::Save(wstring file)
{
	BinaryWriter* w = new BinaryWriter();
	w->Open(file);

	w->String(String::ToString(name));
	w->Float(duration);
	w->Float(frameRate);
	w->UInt(frameCount);

	w->UInt(keyframesCount);

	unordered_map<wstring, ModelKeyframe*>::iterator it;
	for(it = keyframeMap.begin(); it != keyframeMap.end(); it++)
	{
		wstring name = it->second->boneName;
		w->String(String::ToString(name));
		w->UInt(it->second->transforms.size());

		w->Byte(&it->second->transforms[0], sizeof(ModelKeyframe::Transform) * it->second->transforms.size());
	}

	w->Close();
	SAFE_DELETE(w);
}

void ModelClip::PostRender(wstring boneName)
{
	ImGui::TextColored(ImVec4(255, 255, 0, 255), (String::ToString(name) + " Hierarchy").c_str());

	unordered_map<wstring, ModelKeyframe*>::iterator iter;
	//int cnt = 0;
	//for (iter = keyframeMap.begin(); iter != keyframeMap.end();) {
	//	wstring name = iter->first;

	//	if (ImGui::Selectable(String::ToString(name).c_str(), false, 0, ImVec2(250, 20))) {}

	//	ImGui::SameLine(270);
	//	if (ImGui::BeginDragDropTarget())
	//	{
	//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("BoneName"))
	//		{
	//			IM_ASSERT(payload->DataSize == sizeof(string));
	//			string* data = (string*)payload->Data;

	//			keyframeMap[String::ToWString(*data)] = iter->second;

	//			iter = keyframeMap.erase(iter);
	//		}
	//		ImGui::EndDragDropTarget();
	//	}
	//	else if (ImGui::ColorButton(("##keymapframe" + to_string(cnt++)).c_str(),
	//		ImVec4(255, 0, 0, 128), ImGuiColorEditFlags_NoTooltip))
	//	{
	//		iter = keyframeMap.erase(iter);
	//	}
	//	else
	//		iter++;
	//}

	for (int i = 0; i < keyList.size(); i++) {
		ModelKeyframe* modelKeyframe = keyframeMap[keyList[i]];
		wstring name = modelKeyframe->boneName;

		if (ImGui::Selectable(String::ToString(name).c_str(), false, 0, ImVec2(250, 20))) {
			if (boneName != L"") {

				keyframeMap[boneName] = modelKeyframe;

				for (iter = keyframeMap.begin(); iter != keyframeMap.end(); iter++) {
					if (iter->first == keyList[i]) {
						keyframeMap.erase(iter);
						break;
					}
				}

				keyList[i] = boneName;
				keyframeMap[keyList[i]]->boneName = keyList[i];
			}
		}

		ImGui::SameLine(270);
		//if (ImGui::BeginDragDropTarget())
		//{
		//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("BoneName"))
		//	{
		//		IM_ASSERT(payload->DataSize == sizeof(string));
		//		string* data = (string*)payload->Data;

		//		keyframeMap[String::ToWString(*data)] = modelKeyframe;

		//		for (iter = keyframeMap.begin(); iter != keyframeMap.end(); iter++) {
		//			if (iter->first == keyList[i]) {
		//				keyframeMap.erase(iter);
		//				break;
		//			}
		//		}

		//		keyList[i] = String::ToWString(*data);
		//		keyframeMap[keyList[i]]->boneName = keyList[i];
		//	}
		//	ImGui::EndDragDropTarget();
		//}
		
		if (ImGui::ColorButton(("##keymapframe" + to_string(i)).c_str(),
			ImVec4(255, 0, 0, 128), ImGuiColorEditFlags_NoTooltip))
		{
			for (iter = keyframeMap.begin(); iter != keyframeMap.end(); iter++) {
				if (iter->first == keyList[i]) {
					keyframeMap.erase(iter);

					keyList.erase(keyList.begin() + i);

					break;
				}
			}
		}
	}
}
