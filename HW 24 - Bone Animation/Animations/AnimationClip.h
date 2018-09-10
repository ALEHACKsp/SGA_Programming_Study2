#pragma once

#include "Objects\GameModel.h"

#include <set>

struct AnimationData
{
	float maxPlayTime; // 현재 프레임에서 다음 프레임으로 넘어가는데 걸리는 시간

	D3DXVECTOR3 scale;
	D3DXQUATERNION rotation;
	D3DXVECTOR3 translation;
};

class AnimationClip
{
public:
	AnimationClip();
	~AnimationClip();

	void Update();

	void PostRender();

	void SetGameModel(GameModel* model);
	void SetMesh(wstring name);

	void Play();
	void Pause();
	void Stop();
	void Resume();

	void Add();
	void Reset();

	void InitModel();
	void Save(wstring file);
	void Load(wstring file);

	float* GetCurTime() { return &currentTime; }
	float* GetMaxTime() { return &maxTime; }

	void SetMaxTime(float maxTime) { this->maxTime = maxTime; }

private:
	GameModel* model; // 애니메이션을 적용할 게임 모델
	//wstring meshName;
	//vector<AnimationData> aniClip;

	wstring currentMesh;
	vector<wstring> meshNames;
	map < wstring, vector<AnimationData> > animations;

	float currentTime; // 애니메이션의 현재 시간
	float maxTime; // 애니메이션 플레이 총 시간

	bool isPlay;

	set<float> times;
};