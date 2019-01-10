#pragma once

class ProgressBar
{
public:
	static ProgressBar* Get();

	static void Create(float step = 1.0f, float closeTime = 5.0f);
	static void Delete();

	void Set(float ratio);
	void Done();

	void Update();
	void Render();

private:
	ProgressBar();
	~ProgressBar();

	static ProgressBar* instance;

	float step;
	float ratio;

	bool bOpen;
	float deltaTime;
	float closeTime;
};
