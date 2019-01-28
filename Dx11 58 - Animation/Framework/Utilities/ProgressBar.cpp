#include "Framework.h"
#include "ProgressBar.h"

ProgressBar* ProgressBar::instance = NULL;

ProgressBar * ProgressBar::Get()
{
	assert(instance != NULL);

	return instance;
}

void ProgressBar::Create(float step, float closeTime)
{
	instance = new ProgressBar();
	instance->step = step;
	instance->bOpen = true;
	instance->closeTime = closeTime;
}

void ProgressBar::Delete()
{
	SAFE_DELETE(instance);
}

void ProgressBar::Set(float ratio)
{
	this->ratio = ratio;
}

void ProgressBar::Done()
{
	ratio = 1.0f;
	deltaTime = 0;
}

void ProgressBar::Update()
{
	if (bOpen == false) return;

	if (ratio < 1.0f)
		ratio += step * Time::Delta();
	else {
		deltaTime += Time::Delta();
		if (deltaTime >= closeTime)
			bOpen = false;
	}
}

void ProgressBar::Render()
{
	if (bOpen) {
		ImGui::Begin("ProgressBar");
		ImGui::ProgressBar(ratio);
		ImGui::End();
	}
}

ProgressBar::ProgressBar()
{
	ratio = 0;
}

ProgressBar::~ProgressBar()
{
}
