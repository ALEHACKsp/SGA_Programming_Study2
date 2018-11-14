#pragma once

class Program
{
public:
	Program();
	~Program();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen();

private:
	ExecuteValues* values;
	vector<class Execute *> executes;

	bool showDebug;

	float angle1, angle2;
};