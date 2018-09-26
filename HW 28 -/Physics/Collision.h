#pragma once

#include "CapsuleCollider.h"

class Collision
{
public:
	Collision() {}
	~Collision() {}

	static bool IsOverlapCapsuleCapsule(
		D3DXMATRIX* world1, CapsuleCollider* collider1,
		D3DXMATRIX* world2, CapsuleCollider* collider2);
};