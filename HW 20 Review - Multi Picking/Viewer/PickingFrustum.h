#pragma once

class PickingFrustum
{
public:
	PickingFrustum(ExecuteValues* values,
		float zFar, Camera* camera = NULL, Perspective* perspective = NULL);
	~PickingFrustum();

	void Update();
	void Render();

	bool ContainPoint(D3DXVECTOR3& position);

	void SetzFar(float zFar) { this->zFar = zFar; }

private:
	void CalcCrossPoint();

private:
	ExecuteValues* values;

	D3DXPLANE planes[6];

	float zFar;

	Camera* camera;
	Perspective* perspective;

private:
	D3DXVECTOR3 startPos, endPos;
	D3DXVECTOR3 crossPoint[4]; // far�� �浹�ϴ� ������ 4�� (mouse start end point 4��)

	bool bUpdatePlane;

private:
	class Line* line;
	D3DXVECTOR3 linePos[5];

private:
	// linetype 0 ���� ���콺 �巡�� �簢���� line draw, 1 frustum �簢�� �ٱ׸���
	int lineType;
	D3DXVECTOR3 linePos2[24];
	D3DXVECTOR3 crossPoint2[4]; // near ������ 4��
};