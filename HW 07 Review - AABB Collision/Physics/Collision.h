#pragma once

class Collision
{
public:
	Collision();
	~Collision();

	static bool IsOverlap(
		D3DXMATRIX* matWorldA, class CircleCollider* pBoundA,
		D3DXMATRIX* matWorldB, class CircleCollider* pBoundB);

	static bool IsOverlap(
		D3DXMATRIX* matWorldA, class RectCollider* pBoundA,
		D3DXMATRIX* matWorldB, class CircleCollider* pBoundB);

	static bool IsOverlap(
		D3DXMATRIX* matWorldA, class CircleCollider* pBoundA,
		D3DXMATRIX* matWorldB, class RectCollider* pBoundB) {
		return IsOverlap(matWorldB, pBoundB, matWorldA, pBoundA);
	}

	static bool IsOverlap(
		D3DXMATRIX* matWorldA, class RectCollider* pBoundA,
		D3DXMATRIX* matWorldB, class RectCollider* pBoundB);
};