#pragma once

class ModelTweener
{
public:
	ModelTweener();
	~ModelTweener();

	// blendTime = 0 이면 안 섞임
	void Play(class ModelClip* clip, bool bRepeat = false,
		float blendTime = 0.0f, float speed = 1.0f, float startTime = 0.0f);
	void Pause() { isPause = true; }
	void Resume() { isPause = false; }
	bool IsPlay() { return isPlay; }
	bool IsPause() { return isPause; }
	void Stop();

	// update시 섞음
	void UpdateBlending(class ModelBone* bone, float time);

private:
	class ModelClip* current;
	class ModelClip* next;

	float blendTime;
	float elapsedTime; // deltaTime이라고 같음 이전 프레임부터 현재 프레임까지의 시간

	bool isPlay;
	bool isPause;
};
