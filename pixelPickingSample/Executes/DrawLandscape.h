#pragma once
#include "Execute.h"

class DrawLandscape : public Execute
{
public:
	DrawLandscape(ExecuteValues* values);
	~DrawLandscape();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen();

private:
	class Sky* sky;
	class Terrain* terrain;
	class QuadTree* quadTree;
	class MeshCube* cube;
	Shader* shader;
	RenderTarget* renderTarget;
	Render2D* postEffect;
};