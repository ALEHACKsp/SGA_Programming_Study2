#include "Framework.h"
#include "ProgressBar.h"

ProgressBar* ProgressBar::instance = NULL;

ProgressBar * ProgressBar::Get()
{
	assert(instance != NULL);

	return instance;
}

void ProgressBar::Create()
{
	instance = new ProgressBar();
}

void ProgressBar::Delete()
{
	SAFE_DELETE(instance);
}

void ProgressBar::Set(float target, float timeOut)
{
	current = this->target;
	this->target = target;
	this->timeOut = timeOut;
	this->time = Time::Get()->Running() + timeOut;
}

void ProgressBar::Done()
{
	ratio = 1.0f;
}

void ProgressBar::Update()
{
	if (target - ratio <= Math::EPSILON)
		ratio = target;
	else
		ratio = Math::Lerp(current, target, 1.0f - (time - Time::Get()->Running()) / timeOut);
}

void ProgressBar::Render()
{
	ImGui::Begin("ProgressBar");
	ImGui::ProgressBar(ratio);
	ImGui::End();
}

ProgressBar::ProgressBar()
{
	current = 0;
	ratio = 0;
	target = 0;
}

ProgressBar::~ProgressBar()
{
}
