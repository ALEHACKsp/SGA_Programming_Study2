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
	if (clips.size() < 1) 
		__super::Update();

	//CalcPosition();

	//for (UINT i = 0; i < model->BoneCount(); i++)
	//{
	//	ModelBone* bone = model->BoneByIndex(i);

	//	if (bone->Parent() != NULL)
	//	{
	//		int index = bone->Parent()->Index();
	//		// 부모의 local를 곱해야함 
	//		bone->Global(bone->Local() * model->BoneByIndex(index)->Local());
	//	}
	//	else
	//	{
	//		bone->Global(bone->Local());
	//	}
	//}

	//for (UINT i = 0; i < model->BoneCount(); i++)
	//{
	//	ModelBone* bone = model->BoneByIndex(i);
	//	clips[0]->UpdateKeyframe(bone, Time::Delta());
	//}

	//__super::Update();

	CalcPosition();

	D3DXMATRIX t = Transformed();

	boneTransforms.clear();
	boneTransforms.assign(model->BoneCount(), D3DXMATRIX());

	for (size_t i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->BoneByIndex(i);

		if (clips.size() < 1) continue;

		D3DXMATRIX local = bone->Local();
		D3DXMATRIX keyframeMatrix = clips[0]->GetKeyframeMatrix(bone, Time::Delta());

		if (bone->Parent() != NULL)
		{
			int index = bone->Parent()->Index();
			boneTransforms[i] = keyframeMatrix * boneTransforms[index];
		}
		else
		{
			boneTransforms[i] = keyframeMatrix * local * t;
		}
	}
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
