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
	bool LockRoot() { return bLockRoot; }
	void Repeat(bool val) { bRepeat = val; }
	bool Repeat() { return bRepeat; }
	void Speed(float val) { speed = val; }
	float Speed() { return speed; }
	void StartTime(float val) { playTime = val; }
	float PlayTime() { return playTime; }
	float PlayTime(float val) { return playTime = val; }

	unordered_map<wstring, ModelKeyframe*>& KeyframeMap() { return keyframeMap; }
	void GetKeyframeMapTransform(wstring boneName, int frame,
		OUT D3DXVECTOR3* S, OUT D3DXQUATERNION* R, OUT D3DXVECTOR3* T);
	void SetKeyframeMapTransform(wstring boneName, int frame,
		D3DXVECTOR3& S, D3DXQUATERNION& R, D3DXVECTOR3& T);
	void GetKeyframeMapTime(wstring boneName, int frame, OUT float* time);

	void Name(wstring name) { this->name = name; }
	wstring Name() { return name; }

	float Duration() { return duration; }
	UINT FrameCount() { return frameCount; }

	void Save(wstring file);

	void PostRender(wstring boneName = L"");

private:
	wstring name;

	float duration;
	float frameRate;
	UINT frameCount;

	UINT keyframesCount;

	bool bLockRoot;
	bool bRepeat;
	float speed;
	float playTime;

	//vector<class ModelKeyframe *> keyframes; // 너무 느림
	typedef pair<wstring, ModelKeyframe*> Pair;
	unordered_map<wstring, ModelKeyframe*> keyframeMap; // hash table로 바꾸심

	vector<wstring> keyList;
};