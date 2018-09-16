#pragma once

class ModelTweener
{
public:
	ModelTweener();
	~ModelTweener();

	// blendTime = 0 �̸� �� ����
	void Play(class ModelClip* clip, bool bRepeat = false,
		float blendTime = 0.0f, float speed = 1.0f, float startTime = 0.0f);
	void Pause() { isPause = true; }
	void Resume() { isPause = false; }
	bool IsPlay() { return isPlay; }
	bool IsPause() { return isPause; }
	void Stop();

	// update�� ����
	void UpdateBlending(class ModelBone* bone, float time);

private:
	class ModelClip* current;
	class ModelClip* next;

	float blendTime;
	float elapsedTime; // deltaTime�̶�� ���� ���� �����Ӻ��� ���� �����ӱ����� �ð�

	bool isPlay;
	bool isPause;
};
