#include "stdafx.h"
#include "GameAnimModel.h"
#include "../Model/ModelClip.h"
#include "../Model/ModelTweener.h"

GameAnimModel::GameAnimModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: GameModel(matFolder, matFile, meshFolder, meshFile)
{
	model->Buffer()->UseBlend(true);
	tweener = new ModelTweener();
}

GameAnimModel::~GameAnimModel()
{
	for (ModelClip* clip : clips)
		SAFE_DELETE(clip);

	SAFE_DELETE(tweener);
	SAFE_DELETE(shader);
}

void GameAnimModel::Update()
{
	if (clips.size() < 1)
	{
		__super::Update();
		return;
	}

	CalcPosition();

	UINT boneCount = model->BoneCount();
	for (UINT i = 0; i < boneCount; i++)
	{
		ModelBone* bone = model->BoneByIndex(i);
	
		tweener->UpdateBlending(bone, Time::Delta());
	}

	__super::Update();
}

UINT GameAnimModel::AddClip(wstring file, float startTime)
{
	ModelClip* clip = new ModelClip(file);
	ClipBoneNameMatch(clip);
	clips.push_back(clip);

	return clips.size() - 1;
}

void GameAnimModel::ClearClip()
{
	for (ModelClip* clip : clips)
		SAFE_DELETE(clip);
	clips.clear();
}

void GameAnimModel::LockRoot(UINT index, bool val) 
{ 
	clips[index]->LockRoot(val); 
}

void GameAnimModel::Repeat(UINT index, bool val) 
{ 
	clips[index]->Repeat(val); 
}

void GameAnimModel::Speed(UINT index, float val) 
{
	clips[index]->Speed(val); 
}

void GameAnimModel::Play(UINT index, bool bRepeat, float blendTime, float speed, float startTime)
{
	tweener->Play(clips[index], bRepeat, blendTime, speed, startTime);
}

void GameAnimModel::Play()
{
	tweener->Play();
}

void GameAnimModel::Stop()
{
	tweener->Stop();
}

void GameAnimModel::ClipBoneNameMatch(ModelClip * clip)
{
	vector<pair<wstring, ModelKeyframe *>>* preClip = clip->GetKeyframeMap();
	vector<pair<wstring, ModelKeyframe *>> setClip;

	UINT boneCount = GetModel()->BoneCount();
	UINT clipCount = preClip->size();
	for (UINT i = 0; i < boneCount; ++i)
	{
		pair<wstring, ModelKeyframe*> temp = make_pair<wstring, ModelKeyframe*>(GetModel()->Bones()[i]->Name(), NULL);
		for (UINT j = 0; j < clipCount; ++j)
		{
			if (temp.first == (*preClip)[j].first)
			{
				temp.second = (*preClip)[j].second;
				break;
			}
		}
		setClip.push_back(temp);
	}
	preClip->assign(setClip.begin(), setClip.end());
}
