#pragma once
#include "Execute.h"

class TestComputeBlur : public Execute
{
public:
	TestComputeBlur();
	~TestComputeBlur();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}
	
private:
	class Sky* sky;

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