#include "Framework.h"
#include "GameAnimator.h"

#include "../Model/ModelClip.h"
#include "../Model/ModelMesh.h"
#include "../Model/ModelMeshPart.h"

GameAnimator::GameAnimator(wstring shaderFile, wstring materialFile, wstring meshFile)
	: GameModel(shaderFile, materialFile, meshFile)
	, mode(Mode::Stop)
	, currentClip(0), currentKeyframe(0), nextKeyframe(0)
	, frameTime(0.0f), frameFactor(0.0f)
	, playTime(0.0f)
{
	playRate = 1.0f;
	pass = 1;
}

GameAnimator::~GameAnimator()
{
	for (ModelClip* clip : clips)
		SAFE_DELETE(clip);
}

void GameAnimator::Ready()
{
	__super::Ready();
}

void GameAnimator::Update()
{
	__super::Update();

	if (Mode::Play != mode) return;

	frameTime += Time::Delta();
	playTime += Time::Delta();

	ModelClip* clip = clips[currentClip];
	float invFrameRate = 1.0f / clip->FrameRate();

	if (playTime > (invFrameRate / playRate))
	{
		playTime -= (invFrameRate / playRate);
		currentKeyframe = (currentKeyframe + 1) % clip->FrameCount();
		nextKeyframe = (currentKeyframe + 1) % clip->FrameCount();
	}

	frameFactor = playTime / (invFrameRate / playRate);

	if (frameTime > invFrameRate)
	{
		frameTime -= invFrameRate;

		UpdateTransforms();
		MappedBoneBuffer();
		UpdateVertex();
	}
}

void GameAnimator::Render()
{
	ImGui::Begin("Animator");

	ImGui::Text("")

	if (ImGui::Button("Play")) Play();
	ImGui::SameLine(60);
	if (ImGui::Button("Pause")) Pause();
	ImGui::SameLine(120);
	if (ImGui::Button("Stop")) Stop();

	if (ImGui::SliderInt("Frame", &currentKeyframe, 0, clips[currentClip]->FrameCount()))
	{
		currentKeyframe = (currentKeyframe + 1) % clips[currentClip]->FrameCount();
		nextKeyframe = (currentKeyframe + 1) % clips[currentClip]->FrameCount();
		
		frameTime = 0;
		playTime = 0;

		UpdateTransforms();
		MappedBoneBuffer();
		UpdateVertex();
	}

	if (ImGui::DragFloat("Speed", &playRate, 0.1f))
	{
		if (playRate <= 0.0f)
			playRate = 0;
	}

	ImGui::End();

	__super::Render();
}

void GameAnimator::AddClip(wstring clipFile)
{
	clips.push_back(new ModelClip(clipFile));

	UpdateTransforms();
	MappedBoneBuffer();
	UpdateVertex();
}

void GameAnimator::Stop() 
{
	this->mode = Mode::Stop;
	currentKeyframe = 0;
	nextKeyframe = 0;
	playTime = frameTime = 0;

	UpdateTransforms();
	MappedBoneBuffer();
	UpdateVertex();
}

void GameAnimator::UpdateWorld()
{
	GameRender::UpdateWorld();

	for (Material* material : model->Materials())
		material->SetWorld(World());
}

void GameAnimator::UpdateTransforms()
{
	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->BoneByIndex(i);

		D3DXMATRIX transform;
		D3DXMATRIX parentTransform;

		D3DXMATRIX invGlobal = bone->Global();
		D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);

		ModelKeyframe* frame = clips[currentClip]->Keyframe(bone->Name());
		if (frame == NULL) continue;

		D3DXMATRIX S, R, T;

		ModelKeyframeData current = frame->Transforms[currentKeyframe];
		ModelKeyframeData next = frame->Transforms[nextKeyframe];

		D3DXVECTOR3 s1 = current.Scale;
		D3DXVECTOR3 s2 = next.Scale;

		// frameFactor도 lerp해서 넣어줘야함
		D3DXVECTOR3 s;
		D3DXVec3Lerp(&s, &s1, &s2, frameFactor);
		D3DXMatrixScaling(&S, s.x, s.y, s.z);


		D3DXQUATERNION q1 = current.Rotation;
		D3DXQUATERNION q2 = next.Rotation;

		D3DXQUATERNION q;
		D3DXQuaternionSlerp(&q, &q1, &q2, frameFactor);
		D3DXMatrixRotationQuaternion(&R, &q);


		D3DXVECTOR3 t1 = current.Translation;
		D3DXVECTOR3 t2 = next.Translation;

		D3DXVECTOR3 t;
		D3DXVec3Lerp(&t, &t1, &t2, frameFactor);
		D3DXMatrixTranslation(&T, t.x, t.y, t.z);

		transform = S * R * T;

		int parentIndex = bone->ParentIndex();
		if (parentIndex < 0)
			D3DXMatrixIdentity(&parentTransform);
		else
			parentTransform = boneTransforms[parentIndex];

		boneTransforms[i] = transform * parentTransform;
		renderTransforms[i] = invGlobal * boneTransforms[i];
	}
}
