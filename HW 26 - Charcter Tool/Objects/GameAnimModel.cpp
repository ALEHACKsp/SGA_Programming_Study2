#include "stdafx.h"
#include "GameAnimModel.h"

#include "../Model/ModelClip.h"
#include "../Model/ModelTweener.h"


GameAnimModel::GameAnimModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: GameModel(matFolder, matFile, meshFolder, meshFile)
{
	tweener = new ModelTweener();
	selectClip = 0;
}

GameAnimModel::~GameAnimModel()
{
	//for (ModelClip* clip : clips)
	//	SAFE_DELETE(clip);

	SAFE_DELETE(tweener);
}

void GameAnimModel::Update()
{
	if (clips.size() < 1) {
		__super::Update();
		return;
	}

	//CalcPosition();

	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->BoneByIndex(i);
		//clips[0]->UpdateKeyframe(bone, Time::Delta());
		
		tweener->UpdateBlending(bone, Time::Delta());
	}

	__super::Update();
}

void GameAnimModel::PostRender()
{
	__super::PostRender();

	if (isAnimMode && clips.size() != 0) {
		ModelClip* clip = clips[selectClip];

		int frame = (clip->PlayTime() / clip->Duration()) * clip->FrameCount();
		
		for (int i = 0; i < model->BoneCount(); i++) {

			if (i == selectBone) {
				D3DXVECTOR3 scale, translation;
				D3DXQUATERNION rotation;
				clip->GetKeyframeMapTransform(model->BoneByIndex(i)->Name(), frame,
					&scale, &rotation, &translation);

				float pitch, yaw, roll;
				Math::QuatToPitchYawRoll(rotation, pitch, yaw, roll);
				float r[] = { yaw, pitch, roll };

				D3DXMATRIX S, R, T;

				ImGui::Text("Position");
				ImGui::DragFloat3("T", (float*)translation, 0.5f);

				ImGui::Text("Rotation");
				ImGui::DragFloat3("R", r, 0.01f);
				D3DXQuaternionRotationYawPitchRoll(&rotation, r[0], r[1], r[2]);

				ImGui::Text("Scale");
				ImGui::DragFloat3("S", (float*)scale, 0.01f);

				ImGui::Text("Scaling");
				if (ImGui::InputFloat("s", &scale.x)) {
					if (scale.x <= 0) scale.x = 0.01f;
					scale = D3DXVECTOR3(scale.x, scale.x, scale.x);
				}

				clip->SetKeyframeMapTransform(model->BoneByIndex(i)->Name(), frame,
					scale, rotation, translation);
			}
		}
	}
}

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

void GameAnimModel::LockRoot(UINT index, bool val) { clips[index]->LockRoot(val); }

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

void GameAnimModel::RetargetBone()
{
	Model* model = this->GetModel();
	for (int i = 0; i < model->BoneCount(); i++) {
		ModelBone* bone = model->BoneByIndex(i);

		if (bone->Name().find(L":") != wstring::npos)
			bone->Name(bone->Name().substr(bone->Name().find(L":") + 1));
	}
}

void GameAnimModel::RetargetClip()
{
	Model* model = this->GetModel();

	unordered_map<wstring, ModelKeyframe*> keyFrameMap = clips.back()->KeyframeMap();

	unordered_map<wstring, ModelKeyframe*>::iterator iter;
	for (iter = keyFrameMap.begin(); iter != keyFrameMap.end(); iter++) {

		for (int i = 0; i < model->BoneCount(); i++) {
			ModelBone* bone = model->BoneByIndex(i);
				
			wstring name = iter->first.substr(iter->first.find(L":") + 1);

			if (name == bone->Name()) {
				bone->Name(iter->first);
				break;
			}
		}
			
	}
}
