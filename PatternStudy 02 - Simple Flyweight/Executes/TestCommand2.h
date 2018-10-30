#pragma once
#include "Execute.h"

class TestCommand2 : public Execute
{
public:
	TestCommand2(ExecuteValues* values);
	~TestCommand2();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

	void Undo();
	void Redo();

private:
	void ClearUndoStack();
	void ClearRedoStack();

private:
	vector<class Unit*> units;
	class stack<class Command*> undoStack;
	class stack<class Command*> redoStack;

	int selectUnit;
};