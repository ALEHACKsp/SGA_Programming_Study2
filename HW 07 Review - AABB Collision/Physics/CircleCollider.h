#pragma once

#define CIRCLE_SEGMENTS 36

class CircleCollider
{
public:
	CircleCollider();
	~CircleCollider();

	void GetWorldCenterRadius(
		D3DXMATRIX* matWorld, D3DXVECTOR3 * center, float* radius);
	virtual void Render(D3DXMATRIX* matWorld);
	
	virtual void SetBound(const D3DXVECTOR3* pCenter, 
		const D3DXVECTOR3* pHalfSize);

protected:
	class Line* line;
	D3DXVECTOR3 localCenter;
	float radius;
	D3DXVECTOR3 halfSize;
};