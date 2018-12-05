#pragma once
#include "Execute.h"

class TestEnv : public Execute
{
public:
	TestEnv();
	~TestEnv();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}
	
private:
	void BuildGeometry();
	void BuildCubeTexture(UINT width, UINT height);
	void BuildViewMatrix(float x, float y, float z);

private:
	class CubeSky* sky;

	wstring floor;
	wstring stone;
	wstring brick;

	GameModel* bunny;

	Material* sphereMaterial;
	MeshSphere* sphere[10];
	
	MeshSphere* sphere2;
	float sphereDist;

	Material* cylinderMaterial;
	MeshCylinder* cylinder[10];

	Material* boxMaterial;
	MeshCube* box;

	Material* planeMaterial;
	MeshGrid* plane;

	Material* quadMaterial;
	MeshQuad* quad;

	Viewport* viewport;
	DepthStencilView* dsv;

	ID3D11Texture2D* cubeTexture;
	ID3D11RenderTargetView* rtv[6];
	ID3D11ShaderResourceView* srv;

	D3DXMATRIX view[6];
	class Perspective* perspective;

	Render2D* render2D;

	bool bContext;
};