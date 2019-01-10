#pragma once

class GameAnimator : public GameRender
{
public:
	enum class Mode
	{
		Play = 0, Pause, Stop,
	};

public:
	GameAnimator(wstring shaderFile, wstring materialFile, wstring meshFile);
	~GameAnimator();

	void Update();
	void Render();

	void AddClip(wstring clipFile);

	void Play() { this->mode = Mode::Play; }
	void Pause() { this->mode = Mode::Pause; }
	void Stop() { this->mode = Mode::Stop; }

private:
	void UpdateBone();

private:
	class Model* model;

	Mode mode;

	UINT currentClip;
	int currentKeyframe;
	int nextKeyframe;

	float frameTime;
	float frameFactor;

	vector<D3DXMATRIX> boneTransforms;
	vector<D3DXMATRIX> renderTransforms;

	vector<class ModelClip *> clips;
};