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
	void Render2D();

	void ResizeScreen();

private:
	ExecuteValues* values;
	vector<class Execute *> executes;
};