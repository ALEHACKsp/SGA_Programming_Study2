#pragma once

class ProgressBar
{
public:
	static ProgressBar* Get();

	static void Create();
	static void Delete();

	void Set(float target, float timeOut = 30.0f);
	void Done();

	void Update();
	void Render();

private:
	ProgressBar();
	~ProgressBar();

	static ProgressBar* instance;

	float time;
	float timeOut;

	float current;
	float target;
	float ratio;
};
