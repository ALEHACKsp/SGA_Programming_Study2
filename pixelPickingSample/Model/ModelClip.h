#pragma once
#include "ModelKeyframe.h"

class ModelClip
{
public:
	ModelClip(wstring file);
	~ModelClip();

	void Reset();

	D3DXMATRIX GetKeyframeMatrix(class ModelBone* bone, float time);
	D3DXMATRIX GetKeyframeAnimMatrix(class ModelBone* bone);
	D3DXMATRIX SetKeyframeMatrix(class ModelBone* bone, D3DXMATRIX& transform, UINT interplateFrameCount);
	void UpdateKeyframe(class ModelBone* bone, float time);

	void LockRoot(bool val) { bLockRoot = val; }
	void Repeat(bool val) { bRepeat = val; }
	void Speed(float val) { speed = val; }
	void StartTime(float val) { playTime = val; }

	bool CheckLastFrame();

	float GetFrameRate() { return frameRate; }
	UINT GetFrameCount() { return frameCount; }
	float GetDuration() { return duration; }
	float* GetPlayTime() { return &playTime; }

	vector<pair<wstring, ModelKeyframe *>>* GetKeyframeMap() { return &keyframeMap; }
private:
	wstring name;

	float duration;
	float frameRate;
	UINT frameCount;

	bool bLockRoot;
	bool bRepeat;
	float speed;
	float playTime;

	//vector<class ModelKeyframe *> keyframes;
	typedef pair<wstring, ModelKeyframe *> Pair;
	vector<pair<wstring, ModelKeyframe *>> keyframeMap;
};