#pragma once
#include "Execute.h"

// �߸��� ������� MultiPicking �غ� �� ���� picking�� n�� �ݺ��Ͽ� üũ���� ��û ����

class TestMultiPicking2 : public Execute
{
public:
	TestMultiPicking2(ExecuteValues* values);
	~TestMultiPicking2();

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

	bool checkPicking;
	D3DXVECTOR3 startPos, endPos;
	
	bool drawRect;
	class Line* mouseRect;
	D3DXVECTOR3 lines[5];
};