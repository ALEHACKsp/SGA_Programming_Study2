#pragma once
#include "Execute.h"

class AnimationEditor : public Execute
{
public:
	AnimationEditor(ExecuteValues* values);
	~AnimationEditor();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen() {}

private:
	class AnimationTool* tool;
};