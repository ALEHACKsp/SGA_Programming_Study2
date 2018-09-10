#pragma once
#include "Execute.h"

class TestBoneAnimation : public Execute
{
public:
	TestBoneAnimation(ExecuteValues* values);
	~TestBoneAnimation();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	Shader* shader;

	//class Sky* sky;
	vector<class GameModel*> models;

	int selectModel;
	wstring selectModelMesh;

	RenderTarget* renderTarget;
	Render2D* render2D;
	Render2D* postEffect;

	function<void(wstring)> func;

	// test
	//float deltaTime;
	//D3DXVECTOR3 targetPos;
	//D3DXVECTOR3 pos;
	//bool isPlay;

	class AnimationClip* animation;
};