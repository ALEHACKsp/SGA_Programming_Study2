#pragma once

#include "GameModel.h"

class GameAnimModel : public GameModel
{
protected:
	enum class State {
		Idle, Walking, Attack, Hitted, Dying
	};
	enum class Direction {
		LEFT, RIGHT, 
	};

public:
	GameAnimModel
	(
		wstring matFolder, wstring matFile
		, wstring meshFolder, wstring meshFile
	);
	virtual ~GameAnimModel();

	virtual void Update();
	virtual void Render();
	virtual void PostRender();

	UINT AddClip(wstring file);
	UINT AddClip(class ModelClip* clip);

	vector<class ModelClip *>& Clips() { return clips; }
	void DeleteClip(int i) { clips.erase(clips.begin() + i); }
	void DeleteClip(wstring name);

	void LockRoot(UINT index, bool val);
	void Repeat(UINT index, bool val);
	void Speed(UINT index, float val);

	void Play(UINT index, bool bRepeat = false,
		float blendTime = 0.0f, float speed = 1.0f, float startTime = 0.0f);
	bool IsPlay();
	bool IsPause();
	void Pause();
	void Resume();
	void Stop();

	void SelectClip(int selectClip) { this->selectClip = selectClip; }
	
	void SetOtherModel(GameAnimModel* model) { otherModels.push_back(model); }

	CapsuleCollider* GetCollider() { return collider; }
	CapsuleCollider* GetAttackCollider() { return attackCollider; }
	D3DXMATRIX GetColldierBone();
	D3DXMATRIX GetAttackColldierBone();

	void SetState(State state);

protected:
	virtual void Idle();
	virtual void Walking();
	virtual void Attack();
	virtual void Hitted();
	virtual void Dying();

protected:
	vector<class ModelClip *> clips;
	class ModelTweener* tweener;

	int selectClip;

	State state;
	State prevState;

	Direction dir;
	bool isRotate;

	float moveSpeed;

	int attackBone;

	class CapsuleCollider* collider;
	class CapsuleCollider* attackCollider;

	bool showCollider;
	bool showAttackCollider;
	int configCollider;

	vector<GameAnimModel*> otherModels;

	bool isRender;
};