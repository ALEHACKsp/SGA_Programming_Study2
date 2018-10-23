#pragma once
#include "Execute.h"

class TestCommand : public Execute
{
public:
	TestCommand(ExecuteValues* values);
	~TestCommand();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	class Command* buttonX;
	class Command* buttonY;
	class Command* buttonA;
	class Command* buttonB;

	class Command* fire;
	class Command* jump;
	class Command* change;
	class Command* move;

	vector<string> logs;
};