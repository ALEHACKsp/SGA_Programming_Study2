#pragma once

#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "Ray.h"


class Collision
{
public:
	Collision() {}
	~Collision() {}

	static bool IsOverlapCapsuleCapsule(
		D3DXMATRIX* world1, CapsuleCollider* collider1,
		D3DXMATRIX* world2, CapsuleCollider* collider2);
	
	// Ray Capsule 및 Sphere 충돌 방법 2가지 2번째 방법은 더 간단
	// 2번째 방법은 내 코드에서는 효율이 떨어짐 Sphere를 미리 Setup해야 빠를듯
	#define EFFICIENT_RAY_COLLISION

#ifndef EFFICIENT_RAY_COLLISION
	static bool IsOverlapRaySphere(Ray* ray, SphereCollider* collider, OUT float* tMin, OUT float* tMax);
	static bool ISOverlapRayCapsule(Ray* ray, CapsuleCollider* collider,
		OUT D3DXVECTOR3* p1, OUT D3DXVECTOR3* p2, OUT D3DXVECTOR3* n1, OUT D3DXVECTOR3* n2);
#else
	static bool IsOverlapRaySphere(Ray* ray, SphereCollider* collider);
	static bool IsOverlayRayCapsule(Ray* ray, CapsuleCollider* collider);
#endif // !EFFICIENT_RAY_COLLISION

};