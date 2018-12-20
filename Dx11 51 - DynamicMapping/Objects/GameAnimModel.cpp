#include "stdafx.h"
#include "GameAnimModel.h"

#include "../Model/ModelClip.h"
#include "../Model/ModelTweener.h"

GameAnimModel::GameAnimModel(wstring shaderFile, wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: GameModel(shaderFile, matFolder, matFile, meshFolder, meshFile)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(1);

	tweener = new ModelTweener();
}

GameAnimModel::~GameAnimModel()
{
	for (ModelClip* clip : clips)
		SAFE_DELETE(clip);

	SAFE_DELETE(tweener);
}

void GameAnimModel::Update()
{
	if (clips.size() < 1) return;

	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->BoneByIndex(i);
		//clips[0]->UpdateKeyframe(bone, Time::Delta());
		tweener->UpdateBlending(bone, Time::Delta());
	}

	UpdateWorld();
}

UINT GameAnimModel::AddClip(wstring file)
{
	ModelClip* clip = new ModelClip(file);
	clips.push_back(clip);

	return clips.size() - 1;
}

void GameAnimModel::LockRoot(UINT index, bool val) { clips[index]->LockRoot(val); }

void GameAnimModel::Repeat(UINT index, bool val) { clips[index]->Repeat(val); }

void GameAnimModel::Speed(UINT index, float val) { clips[index]->Speed(val); }

void GameAnimModel::Play(UINT index, bool bRepeat, float blendTime, float speed, float startTime)
{
	tweener->Play(clips[index], bRepeat, blendTime, speed, startTime);
}
