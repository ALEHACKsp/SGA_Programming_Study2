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

	D3DXVECTOR3 camStartPos, camPos2;
	D3DXVECTOR2 camStartRot, camRot2;

	class Freedom * freedom;
	class Orbit * orbit;

	int cameraState;
};