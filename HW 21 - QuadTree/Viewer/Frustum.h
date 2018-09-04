#pragma once

class Frustum
{
public:
	Frustum(ExecuteValues* values,
		float zFar, Camera* camera = NULL, Perspective* perspective = NULL);
	~Frustum();

	void Update();
	void Render();

	bool ContainPoint(D3DXVECTOR3& position);
	bool ContainCube(D3DXVECTOR3& center, float radius);

private:
	void CalcCrossPoint();

private:
	ExecuteValues* values;

	D3DXPLANE planes[6];

	float zFar;

	Camera* camera;
	Perspective* perspective;

private:
	class Line* line;
	D3DXVECTOR3 linePos[24];
	D3DXVECTOR3 crossPoint[8]; // near far 교차점 8개

	DepthStencilState* depthMode[2];
};