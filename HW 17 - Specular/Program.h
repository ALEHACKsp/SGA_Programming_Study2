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

	int camKind;
	class Freedom * freedom;
	class Orbit * orbit;
};