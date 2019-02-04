#pragma once

class GameAnimator : public GameModel
{
public:
	enum class Mode
	{
		Play = 0, Pause, Stop,
	};

public:
	GameAnimator(wstring shaderFile, wstring materialFile, wstring meshFile);
	~GameAnimator();

	void Ready() override;
	void Update();
	void Render() override;

	void AddClip(wstring clipFile);

	void Play() { mode = Mode::Play; }
	void Pause() { this->mode = Mode::Pause; }
	void Stop();

protected:
	void UpdateWorld() override;
	void UpdateTransforms() override;

private:
	Mode mode;

	UINT currentClip;
	int currentKeyframe;
	int nextKeyframe;

	float playRate;

	float frameTime;
	float playTime;
	float frameFactor;

	vector<class ModelClip *> clips;
};