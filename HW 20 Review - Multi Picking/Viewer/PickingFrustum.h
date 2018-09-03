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
	D3DXVECTOR3 crossPoint[4]; // far와 충돌하는 교차점 4개 (mouse start end point 4개)

	bool bUpdatePlane;

private:
	class Line* line;
	D3DXVECTOR3 linePos[5];

private:
	// linetype 0 기존 마우스 드래그 사각형만 line draw, 1 frustum 사각형 다그리기
	int lineType;
	D3DXVECTOR3 linePos2[24];
	D3DXVECTOR3 crossPoint2[4]; // near 교차점 4개
};