#include "stdafx.h"
#include "ModelTweener.h"
#include "ModelClip.h"

ModelTweener::ModelTweener()
	: current(NULL), next(NULL), blendTime(0.0f), elapsedTime(0.0f)
	, isPlay(false), isPause(true)
{

}

ModelTweener::~ModelTweener()
{
}

void ModelTweener::Play(ModelClip * clip, bool bRepeat, float blendTime, float speed, float startTime)
{
	this->blendTime = blendTime;
	this->elapsedTime = startTime;
	isPlay = true;
	isPause = false;

	if (next == NULL)
	{
		next = clip;
		next->Repeat(bRepeat);
		next->StartTime(startTime);
		next->Speed(speed);
	}
	else
	{
		current = clip;
		current->Repeat(bRepeat);
		current->StartTime(startTime);
		current->Speed(speed);
	}
}

void ModelTweener::Stop() {
	if (current != NULL)
		this->current->Reset();
	if (next != NULL)
		this->next->Reset();
	isPlay = false;
	isPause = true;
}

void ModelTweener::UpdateBlending(ModelBone * bone, float time)
{
	if (isPlay == false && isPause == true) {
		return;
	}
	if (isPause)
		time = 0;

	D3DXMATRIX matrix;
	if (next != NULL)
	{
		elapsedTime += time;

		// 입력받은 시간이랑 현재시간의 비율
		float t = elapsedTime / blendTime;

		// 동작 전환 완료
		if (t > 1.0f)
		{
			matrix = next->GetKeyframeMatrix(bone, time);

			current = next;
			next = NULL;
		}
		else
		{
			D3DXMATRIX start = current->GetKeyframeMatrix(bone, time);
			D3DXMATRIX end = next->GetKeyframeMatrix(bone, time);

			Math::LerpMatrix(matrix, start, end, t);
		}
	}
	else
	{
		matrix = current->GetKeyframeMatrix(bone, time);
	}

	bone->Local(matrix);
}
