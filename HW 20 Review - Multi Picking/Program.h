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

	class Camera* camera[2];
	class Perspective* perspective[2];
	class Viewport* viewport[2];
	int selectCam;
};