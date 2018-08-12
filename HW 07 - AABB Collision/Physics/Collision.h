#pragma once

class Collision
{
public:
	Collision();
	~Collision();

	static bool IsOverlapCircleCircle(
		D3DXMATRIX* matWorldA, class CircleCollider* pBoundA,
		D3DXMATRIX* matWorldB, class CircleCollider* pBoundB);

	static bool IsOverlapRectCircle(
		D3DXMATRIX* matWorldA, class RectCollider* pBoundA,
		D3DXMATRIX* matWorldB, class CircleCollider* pBoundB);

	static bool IsOverlapRectRect(
		D3DXMATRIX* matWorldA, class RectCollider* pBoundA,
		D3DXMATRIX* matWorldB, class RectCollider* pBoundB);
};