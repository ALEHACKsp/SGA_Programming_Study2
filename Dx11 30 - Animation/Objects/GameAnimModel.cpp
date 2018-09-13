#include "stdafx.h"
#include "GameAnimModel.h"

#include "../Model/ModelClip.h"

GameAnimModel::GameAnimModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: GameModel(matFolder, matFile, meshFolder, meshFile)
{
}

GameAnimModel::~GameAnimModel()
{
	for (ModelClip* clip : clips)
		SAFE_DELETE(clip);
}

void GameAnimModel::Update()
{
	if (clips.size() < 1) return;

	//CalcPosition();

	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->BoneByIndex(i);
		clips[0]->UpdateKeyframe(bone, Time::Delta());
	}

	__super::Update();
}

UINT GameAnimModel::AddClip(wstring file)
{
	ModelClip* clip = new ModelClip(file);

	return AddClip(clip);
}

UINT GameAnimModel::AddClip(ModelClip * clip)
{
	clips.push_back(clip);

	return clips.size() - 1;
}

void GameAnimModel::LockRoot(UINT index, bool val) { clips[index]->LockRoot(val); }

void GameAnimModel::Repeat(UINT index, bool val) { clips[index]->Repeat(val); }

void GameAnimModel::Speed(UINT index, float val) { clips[index]->Speed(val); }
