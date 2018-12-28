#pragma once
#include "Execute.h"

class MapEditor : public Execute
{
public:
	MapEditor(ExecuteValues* values);
	~MapEditor();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen() {}

private:
	class MapTool* tool;
};