#pragma once

#include "Objects\GameModel.h"

#include <set>

struct AnimationData
{
	float maxPlayTime; // ���� �����ӿ��� ���� ���������� �Ѿ�µ� �ɸ��� �ð�

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
	GameModel* model; // �ִϸ��̼��� ������ ���� ��
	//wstring meshName;
	//vector<AnimationData> aniClip;

	wstring currentMesh;
	vector<wstring> meshNames;
	map < wstring, vector<AnimationData> > animations;

	float currentTime; // �ִϸ��̼��� ���� �ð�
	float maxTime; // �ִϸ��̼� �÷��� �� �ð�

	bool isPlay;

	set<float> times;
};