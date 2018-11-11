#include "stdafx.h"
#include "GameAnimModel.h"

#include "../Model/ModelClip.h"
#include "../Model/ModelTweener.h"

#include "../Physics/CapsuleCollider.h"

GameAnimModel::GameAnimModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: GameModel(matFolder, matFile, meshFolder, meshFile)
{
	tweener = new ModelTweener();
	selectClip = 0;

	state = State::Idle;

	collider = new CapsuleCollider();

	showCollider = false;
	showAttackCollider = false;
	configCollider = 0;

	attackBone = 0;

	isRender = true;
}

GameAnimModel::~GameAnimModel()
{
	for (ModelClip* clip : clips)
		SAFE_DELETE(clip);

	SAFE_DELETE(tweener);

	SAFE_DELETE(collider);
	SAFE_DELETE(attackCollider);
}

void GameAnimModel::Update()
{
	if (clips.size() < 1) {
		__super::Update();
		return;
	}
	
	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->BoneByIndex(i);
		
		tweener->UpdateBlending(bone, Time::Delta());
	}

	prevState = state;

	__super::Update();

	collider->Update(GetColldierBone());

	attackCollider->Update(GetAttackColldierBone());
}

void GameAnimModel::Render()
{
	if (isRender == false)
		return;

	__super::Render();
	
	if (showCollider)
		collider->Render();
	if (showAttackCollider)
		attackCollider->Render();
}

void GameAnimModel::PostRender()
{
	__super::PostRender();

	ImGui::DragFloat("MoveSpeed", &moveSpeed, 0.1f);

	ImGui::Begin("Collider");
	ImGui::Checkbox("show Collider", &showCollider);
	ImGui::Checkbox("show Attack Collider", &showAttackCollider);
	
	ImGui::RadioButton("Collider", &configCollider, 0);
	ImGui::SameLine(120);
	ImGui::RadioButton("AtkCollider", &configCollider, 1);

	if (configCollider == 0)
		collider->PostRender();
	if (configCollider == 1)
		attackCollider->PostRender();
	ImGui::End();
}

#pragma region Animation Control

UINT GameAnimModel::AddClip(wstring file)
{
	ModelClip* clip = new ModelClip(file);
	clips.push_back(clip);

	return clips.size() - 1;
}

UINT GameAnimModel::AddClip(ModelClip * clip)
{
	clips.push_back(clip);

	return clips.size() - 1;
}

void GameAnimModel::DeleteClip(wstring name)
{
	for (int i = 0; i < clips.size(); i++) {
		if (clips[i]->Name() == name) {
			clips.erase(clips.begin() + i);
		}
	}
}

void GameAnimModel::Repeat(UINT index, bool val) { clips[index]->Repeat(val); }

void GameAnimModel::Speed(UINT index, float val) { clips[index]->Speed(val); }

void GameAnimModel::Play(UINT index, bool bRepeat, float blendTime, float speed, float startTime)
{
	tweener->Play(clips[index], bRepeat, blendTime, speed, startTime);
}

bool GameAnimModel::IsPlay()
{
	return tweener->IsPlay();
}

bool GameAnimModel::IsPause()
{
	return tweener->IsPause();
}

void GameAnimModel::Pause()
{
	tweener->Pause();
}

void GameAnimModel::Resume()
{
	tweener->Resume();
}

void GameAnimModel::Stop()
{
	tweener->Stop();
	ResetBoneLocal();
}

void GameAnimModel::LockRoot(UINT index, bool val) { clips[index]->LockRoot(val); }

#pragma endregion

D3DXMATRIX GameAnimModel::GetColldierBone()
{
	D3DXMATRIX boneMatrix;

	if (selectBone != -1)
		boneMatrix = model->BoneByIndex(selectBone)->Global() * boneTransforms[selectBone];
	else
		boneMatrix = model->BoneByIndex(0)->Global() * boneTransforms[0];

	return boneMatrix;
}

D3DXMATRIX GameAnimModel::GetAttackColldierBone()
{
	D3DXMATRIX boneMatrix;

	boneMatrix = model->BoneByIndex(attackBone)->Global() * boneTransforms[attackBone];

	return boneMatrix;
}

void GameAnimModel::SetState(State state)
{
	switch (state)
	{
		case GameAnimModel::State::Idle:
			break;
		case GameAnimModel::State::Walking:
			break;
		case GameAnimModel::State::Attack:
			break;
		case GameAnimModel::State::Hitted:
			Hitted();
			this->state = state;
			break;
		case GameAnimModel::State::Dying:
			break;
	}
}

#pragma region State Animate
void GameAnimModel::Idle()
{
	if (prevState != State::Idle) {
		this->Play((int)State::Idle, true, 30, 5);
	}
}

void GameAnimModel::Walking()
{
	if (prevState != State::Walking)
		this->Play((int)State::Walking, true, 30, 5);
}

void GameAnimModel::Attack()
{
	if (prevState != State::Attack)
		this->Play((int)State::Attack, false, 30, 10);
}

void GameAnimModel::Hitted()
{
	if (prevState != State::Hitted)
		this->Play((int)State::Hitted, false, 30, 10);
}

void GameAnimModel::Dying()
{
	if (prevState != State::Dying)
		this->Play((int)State::Dying, false, 30, 10);
}
#pragma endregion
