#pragma once

#include "Line.h"

class DebugDrawCube : public Line
{
public:
	DebugDrawCube(D3DXVECTOR3& center, D3DXVECTOR3& halfSize);
	~DebugDrawCube();

	void Render();
	void PostRender();

	void GetPosition(D3DXVECTOR3* pCenter, D3DXVECTOR3* pHalfSize) {
		pCenter = &center;
		pHalfSize = &halfSize;
	}
	void SetPosition(D3DXVECTOR3& center, D3DXVECTOR3& halfSize);

	void Set(D3DXVECTOR3& center, D3DXVECTOR3& halfSize, D3DXCOLOR& color);

	void CreateVertex(D3DXVECTOR3* lines);
	
private:
	D3DXVECTOR3 min;
	D3DXVECTOR3 max;
	D3DXVECTOR3 center;
	D3DXVECTOR3 halfSize;
};