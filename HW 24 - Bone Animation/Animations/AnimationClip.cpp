#include <stdafx.h>
#include "AnimationClip.h"

#include "../Utilities/Xml.h"

AnimationClip::AnimationClip()
	:model(NULL), isPlay(false), currentTime(0), maxTime(30)
	, currentMesh(L"")
{
}

AnimationClip::~AnimationClip()
{
}

void AnimationClip::Update()
{
	if (isPlay == false) return;

	if (currentTime < maxTime) {
		currentTime += Time::Delta();
		
		bool isStop = true;
		for (int j = 0; j < meshNames.size(); j++) {
			vector<AnimationData> aniClip = animations[meshNames[j]];

			if (aniClip.size() == 0) continue;

			for (int i = 1; i < aniClip.size(); i++) {
				if (currentTime < aniClip[i].maxPlayTime) {
					AnimationData prevData = aniClip[i - 1];
					float prevTime = aniClip[i - 1].maxPlayTime;

					AnimationData data = aniClip[i];
					float t = (currentTime - prevTime) / (data.maxPlayTime - prevData.maxPlayTime);

					ModelBone* bone = model->GetModel()->BoneByName(meshNames[j]);

					D3DXVECTOR3 scale, translation;
					D3DXQUATERNION rotation;

					D3DXVec3Lerp(&scale, &prevData.scale, &data.scale, t);
					D3DXVec3Lerp(&translation, &prevData.translation, &data.translation, t);
					D3DXQuaternionSlerp(&rotation, &prevData.rotation, &data.rotation, t);

					float pitch, yaw, roll;
					Math::QuatToPitchYawRoll(rotation, pitch, yaw, roll);

					D3DXMATRIX S, R, T;

					D3DXMatrixTranslation(&T, translation.x, translation.y, translation.z);
					D3DXMatrixRotationYawPitchRoll(&R, yaw, pitch, roll);
					D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);

					bone->Local(S * R * T);
					break;
				}
			}

			if (currentTime < aniClip.back().maxPlayTime)
				isStop = false;
		}

		if (isStop)
			Stop();
	}
}

void AnimationClip::PostRender()
{
	set<float>::iterator iter;
	float fTimes[100];
	int index = 0;
	for (iter = times.begin(); iter != times.end(); iter++)
		fTimes[index++] = *iter;

	ImGui::PlotHistogram("Times", (float*)fTimes, times.size());
	

	for (int j = 0; j < meshNames.size(); j++) {
		ImGui::Separator();

		ImGui::TextColored(ImVec4(1,1,0,1), "%s's Size : %d", 
			String::ToString(meshNames[j]).c_str(), animations[meshNames[j]].size());

		vector<AnimationData> clip = animations[meshNames[j]];
		for (int i = 0; i < clip.size(); i++) {
			ImGui::Text("%d data", i);
			AnimationData data = clip[i];
			ImGui::Text("time %f", data.maxPlayTime);
			ImGui::Text("scale %.2f, %.2f %.2f", data.scale.x, data.scale.y, data.scale.z);
			ImGui::Text("rotation %.2f, %.2f %.2f %.2f",
				data.rotation.x, data.rotation.y, data.rotation.z, data.rotation.w);
			ImGui::Text("translation %.2f, %.2f %.2f", data.translation.x, data.translation.y, data.translation.z);
			
		}

	}
}

void AnimationClip::SetGameModel(GameModel * model)
{
	this->model = model;

	meshNames = model->GetModel()->GetMesheNames();
}

void AnimationClip::SetMesh(wstring name)
{
	currentMesh = name;

	if(animations[currentMesh].size() == 0)
		Add();
}

void AnimationClip::Play()
{
	isPlay = true;
	currentTime = 0;

	InitModel();
}

void AnimationClip::Pause()
{
	isPlay = false;
}

void AnimationClip::Stop()
{
	isPlay = false;

	currentTime = 0;

	InitModel();
}

void AnimationClip::Resume()
{
	isPlay = true;
}

void AnimationClip::Add()
{
	if (currentMesh == L"")
		return;

	AnimationData data;
	data.maxPlayTime = animations[currentMesh].size() == 0 ? 0 : currentTime;

	times.insert(data.maxPlayTime);

	ModelBone* bone = model->GetModel()->BoneByName(currentMesh);
	D3DXMATRIX matWorld = bone->Local();

	D3DXVECTOR3 scale, translation;
	D3DXQUATERNION rotation;
	D3DXMatrixDecompose(&scale, &rotation, &translation,
		&matWorld);

	data.scale = scale;
	data.rotation = rotation;
	data.translation = translation;

	float deltaTime = 0;
	vector<AnimationData>::iterator iter;
	bool isAdd = false;
	for (iter = animations[currentMesh].begin(); iter != animations[currentMesh].end(); iter++) {
		deltaTime += (*iter).maxPlayTime;
		if (deltaTime < currentTime)
			continue;

		animations[currentMesh].insert(iter, data);
		isAdd = true;
		break;
	}

	if (!isAdd)
		animations[currentMesh].push_back(data);
}

void AnimationClip::Reset()
{

	InitModel();

	map< wstring, vector<AnimationData> >::iterator iter;

	for (iter = animations.begin(); iter != animations.end(); iter++)
	{
		iter->second.clear();
	}

	times.clear();
}

void AnimationClip::InitModel()
{
	for (int j = 0; j < meshNames.size(); j++) {
		vector<AnimationData> aniClip = animations[meshNames[j]];

		if (aniClip.size() == 0) continue;
		
		ModelBone* bone = model->GetModel()->BoneByName(meshNames[j]);

		D3DXVECTOR3 scale, translation;
		D3DXQUATERNION rotation;

		scale = aniClip[0].scale;
		rotation = aniClip[0].rotation;
		translation = aniClip[0].translation;

		float pitch, yaw, roll;
		Math::QuatToPitchYawRoll(rotation, pitch, yaw, roll);

		D3DXMATRIX S, R, T;

		D3DXMatrixTranslation(&T, translation.x, translation.y, translation.z);
		D3DXMatrixRotationYawPitchRoll(&R, yaw, pitch, roll);
		D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);

		bone->Local(S * R * T);
	}
}

void AnimationClip::Save(wstring fileName)
{
	Xml::XMLDocument * document = new Xml::XMLDocument();
	Xml::XMLDeclaration *decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement((model->GetName() + "AniClip").c_str());
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	for (int i = 0; i < meshNames.size(); i++) {
		Xml::XMLElement* node = document->NewElement("MeshNames");
		node->SetText(String::ToString(meshNames[i]).c_str());
		root->LinkEndChild(node); // 뒤에 붙이려고 하는거

		Xml::XMLElement * index = NULL;
		
		vector<AnimationData> datas = animations[meshNames[i]];
		for (int j = 0; j < datas.size(); j++) {
			index = document->NewElement(("Data_" + to_string(j)).c_str());
			node->LinkEndChild(index);

			Xml::XMLElement * element = NULL;

			element = document->NewElement("maxPlayTime");
			element->SetText(datas[j].maxPlayTime);
			index->LinkEndChild(element);

			element = document->NewElement("scaleX");
			element->SetText(datas[j].scale.x);
			index->LinkEndChild(element);
			element = document->NewElement("scaleY");
			element->SetText(datas[j].scale.y);
			index->LinkEndChild(element);
			element = document->NewElement("scaleZ");
			element->SetText(datas[j].scale.z);
			index->LinkEndChild(element);

			element = document->NewElement("rotationX");
			element->SetText(datas[j].rotation.x);
			index->LinkEndChild(element);
			element = document->NewElement("rotationY");
			element->SetText(datas[j].rotation.y);
			index->LinkEndChild(element);
			element = document->NewElement("rotationZ");
			element->SetText(datas[j].rotation.z);
			index->LinkEndChild(element);
			element = document->NewElement("rotationW");
			element->SetText(datas[j].rotation.w);
			index->LinkEndChild(element);

			element = document->NewElement("translationX");
			element->SetText(datas[j].translation.x);
			index->LinkEndChild(element);
			element = document->NewElement("translationY");
			element->SetText(datas[j].translation.y);
			index->LinkEndChild(element);
			element = document->NewElement("translationZ");
			element->SetText(datas[j].translation.z);
			index->LinkEndChild(element);
		}
	}

	string file = String::ToString(fileName);
	document->SaveFile(file.c_str());

	SAFE_DELETE(document);
}

void AnimationClip::Load(wstring file)
{
	Xml::XMLDocument * document = new Xml::XMLDocument();

	wstring path = file;

	Xml::XMLError error = document->LoadFile(
		String::ToString(path).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();
	Xml::XMLElement* meshNode = root->FirstChildElement();

	do
	{
		vector<AnimationData> datas;

		Xml::XMLElement* indexNode = NULL;

		indexNode = meshNode->FirstChildElement();

		while(indexNode != NULL)
		{
			AnimationData data;

			Xml::XMLElement* node = NULL;
			node = indexNode->FirstChildElement();

			data.maxPlayTime = node->FloatText();

			node = node->NextSiblingElement();
			data.scale.x = node->FloatText();
			node = node->NextSiblingElement();
			data.scale.y = node->FloatText();
			node = node->NextSiblingElement();
			data.scale.z = node->FloatText();

			node = node->NextSiblingElement();
			data.rotation.x = node->FloatText();
			node = node->NextSiblingElement();
			data.rotation.y = node->FloatText();
			node = node->NextSiblingElement();
			data.rotation.z = node->FloatText();
			node = node->NextSiblingElement();
			data.rotation.w = node->FloatText();

			node = node->NextSiblingElement();
			data.translation.x = node->FloatText();
			node = node->NextSiblingElement();
			data.translation.y = node->FloatText();
			node = node->NextSiblingElement();
			data.translation.z = node->FloatText();

			datas.push_back(data);

			indexNode = indexNode->NextSiblingElement();
		};

		string meshName = meshNode->GetText();
		meshName = String::Trim(meshName);

		animations[String::ToWString(meshName)] = datas;

		meshNode = meshNode->NextSiblingElement();
	} while (meshNode != NULL);

	SAFE_DELETE(document);
}
