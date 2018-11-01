#pragma once
#include "Execute.h"

class TestRandomChaosClouds : public Execute
{
public:
	enum class SkyType { SpotClouds, CloudsSky, CloudField, CloudSplatter };

public:
	TestRandomChaosClouds(ExecuteValues* values);
	~TestRandomChaosClouds();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:

	vector<int> allCloudSprites;

	float speedTran;
	float speedRot;

	int select;
	class InstancedCloudManager* cloudManager[4];
};