#pragma once

#include "ModelKeyframe.h"

class ModelClip
{
public:
	ModelClip(wstring file);
	~ModelClip();

	void Reset();
	
	D3DXMATRIX GetKeyframeMatrix(class ModelBone* bone, float time);

	void UpdateKeyframe(ModelBone * bone, float time);

	void LockRoot(bool val) { bLockRoot = val; }
	void Repeat(bool val) { bRepeat = val; }
	void Speed(float val) { speed = val; }

private:
	wstring name;

	float duration;
	float frameRate;
	UINT frameCount;

	bool bLockRoot;
	bool bRepeat;
	float speed;
	float playTime;

	//vector<class ModelKeyframe *> keyframes; // 너무 느림
	typedef pair<wstring, ModelKeyframe*> Pair;
	unordered_map<wstring, ModelKeyframe*> keyframeMap; // hash table로 바꾸심
};