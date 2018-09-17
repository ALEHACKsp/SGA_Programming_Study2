#pragma once
#include "Execute.h"
#include "../Fbx/Exporter.h"

class ExRetargeting : public Execute
{
public:
	ExRetargeting(ExecuteValues* values);
	~ExRetargeting();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

	void ReadModel(wstring file);
	void ReadAnimation(wstring file);

	void SaveAnimation(wstring file);

private:
	vector<class GameAnimModel*> models;
	vector<class ModelClip*> clips;

	Shader* shader;

	function<void(wstring)> func;

	Fbx::Exporter* exporter;

	bool isAnimMode;
	bool bLockRoot;
	bool bRepeat;
	float clipSpeed;
	float startTime;

	class Camera* freedomCamera;
	class Orbit* orbitCamera;
};