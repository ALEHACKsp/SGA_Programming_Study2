#pragma once
#include "Execute.h"

class TestMultiPicking : public Execute
{
public:
	TestMultiPicking(ExecuteValues* values);
	~TestMultiPicking();

	void Update();
	void PreRender() {}
	void Render();
	void PostRender() {}
	void ResizeScreen() {}

private:
	Shader* shader;

	UINT drawCount;
	vector<D3DXVECTOR3> positions; // �Ǵ��غ� ��ġ
	vector<D3DXCOLOR> colors;

	class MeshPlane* plane;
	class MeshSphere* sphere;

	class Frustum* frustum;

	class Camera* camera;
	class Perspective* projection;

	class Frustum* pickingFrustum;
	class Camera* pickingCamera;
	class Perspective* pickingProjection;
	bool checkPicking;
	D3DXVECTOR3 startPos, endPos;
};