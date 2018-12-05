#pragma once
#include "Execute.h"

class TestCubeMap : public Execute
{
public:
	TestCubeMap();
	~TestCubeMap();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}
	
private:
	class CubeSky* sky;

	wstring floor;
	wstring stone;
	wstring brick;

	GameModel* bunny;

	Material* sphereMaterial;
	MeshSphere* sphere[10];

	Material* cylinderMaterial;
	MeshCylinder* cylinder[10];

	Material* boxMaterial;
	MeshCube* box;

	Material* planeMaterial;
	MeshGrid* plane;

	Material* quadMaterial;
	MeshQuad* quad;

	Viewport* viewport;
	RenderTargetView* rtv;
	DepthStencilView* dsv;

	Render2D* render2D;
};