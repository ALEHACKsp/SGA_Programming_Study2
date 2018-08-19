#pragma once

enum COLLIDER_TYPE { COLLIDER_NONE, COLLIDER_BOX, COLLIDER_SPHERE };

class Collider
{
public:
	Collider(COLLIDER_TYPE type = COLLIDER_NONE);
	~Collider();

	COLLIDER_TYPE GetColliderType() { return type; }

	void GetWorldBox(D3DXMATRIX* matWorld, D3DXVECTOR3* outBoxPos);
	void GetWorldAABBMinMax(D3DXMATRIX* matWorld,
		D3DXVECTOR3 * min, D3DXVECTOR3 * max);

	void Render(D3DXMATRIX* matWorld);

	void SetBound(const D3DXVECTOR3* pCenter, const D3DXVECTOR3* pHalfSize);

	// circle collider 시 필요한 거
	void GetWorldCenterRadius(
		D3DXMATRIX* matWorld, D3DXVECTOR3* center, float * radius);

	void CreateBoundBox();
	vector<D3DXVECTOR3>& GetBound() { return vecBound; }

	void CreateAABBBox();
	vector<D3DXVECTOR3>& GetAABBBound() { return vecAABBBound; }

private:
	COLLIDER_TYPE type;

	D3DXVECTOR3 localMinPos;
	D3DXVECTOR3 localMaxPos;

	vector<D3DXVECTOR3> vecBound;
	vector<D3DXVECTOR3> vecAABBBound;

// circle Collider 시 필요한거
private:
	class Line* line;
	D3DXVECTOR3 localCenter;
	float radius;
	D3DXVECTOR3 halfSize;
};