#include "stdafx.h"
#include "ModelClip.h"
#include "ModelBone.h"
#include "../Utilities/BinaryFile.h"

ModelClip::ModelClip(wstring file)
	: playTime(0.0f)
	, bRepeat(false), speed(1.0f), bLockRoot(false)
{
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	name = String::ToWString(r->String());
	duration = r->Float();
	frameRate = r->Float();
	frameCount = r->UInt();

	UINT keyframesCount = r->UInt();
	for (UINT i = 0; i < keyframesCount; i++)
	{
		ModelKeyframe* keyframe = new ModelKeyframe();
		keyframe->boneName = String::ToWString(r->String());

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

		keyframeMap.push_back(Pair(keyframe->boneName, keyframe));
	}

	r->Close();
	SAFE_DELETE(r);
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
	if (keyframeMap[bone->Index()].second == NULL)
	{
		D3DXMATRIX temp;
		D3DXMatrixIdentity(&temp);

		return temp;
	}
	ModelKeyframe* keyframe = keyframeMap[bone->Index()].second;

	playTime += speed * time;
	if (bRepeat == true)
	{
		if (playTime >= duration)
		{
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
	if (parentIndex < 0)
	{
		if (bLockRoot == true)
			D3DXMatrixIdentity(&parent);
		else
			parent = animation;
	}
	else
	{
		parent = animation * bone->Parent()->Global();
	}

	return invGlobal * parent;
}

D3DXMATRIX ModelClip::GetKeyframeAnimMatrix(ModelBone * bone)
{
	if (keyframeMap[bone->Index()].second == NULL)
	{
		D3DXMATRIX temp;
		D3DXMatrixIdentity(&temp);

		return temp;
	}
	ModelKeyframe* keyframe = keyframeMap[bone->Index()].second;

	D3DXMATRIX invGlobal = bone->Global();
	D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);

	return keyframe->GetInterpolatedMatrix(playTime, bRepeat);
}

D3DXMATRIX ModelClip::SetKeyframeMatrix(ModelBone * bone, D3DXMATRIX& transform, UINT interplateFrameCount)
{
	if (keyframeMap[bone->Index()].second == NULL)
	{
		D3DXMATRIX temp;
		D3DXMatrixIdentity(&temp);

		return temp;
	}
	ModelKeyframe* keyframe = keyframeMap[bone->Index()].second;
		
	keyframe->SetBoneTransforms(transform, playTime, interplateFrameCount);
}

void ModelClip::UpdateKeyframe(ModelBone * bone, float time)
{
	D3DXMATRIX animation = GetKeyframeMatrix(bone, time);

	bone->Local(animation);
}

bool ModelClip::CheckLastFrame()
{
	float frame = duration / (float)frameCount;
	return *GetPlayTime() > duration - frame;
}
