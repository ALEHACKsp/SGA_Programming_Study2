#pragma once
#include "Execute.h"

class TestFrustum : public Execute
{
public:
	TestFrustum(ExecuteValues* values);
	~TestFrustum();

	void Update();
	void PreRender() {}
	void Render();
	void PostRender() {}
	void ResizeScreen() {}

private:
	Shader* shader;

	UINT drawCount;
	vector<D3DXVECTOR3> positions; // 판단해볼 위치

	class MeshPlane* plane;
	class MeshSphere* sphere;

	class Frustum* frustum;

	class Camera* camera;
	class Perspective* projection;

	//float w, h;
	//float zFar;
	float fov;
};