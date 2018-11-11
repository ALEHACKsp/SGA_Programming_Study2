#include "stdafx.h"
#include "Collision.h"	

bool Collision::IsOverlapCapsuleCapsule(D3DXMATRIX * world1, CapsuleCollider * collider1, D3DXMATRIX * world2, CapsuleCollider * collider2)
{
	float s, t;
	D3DXVECTOR3 c1, c2;

	D3DXVECTOR3 a1, b1, a2, b2;
	float radius1, radius2;

	collider1->Get(&a1, &b1, &radius1);
	collider2->Get(&a2, &b2, &radius2);

	//a1 = D3DXVECTOR3(0,  collider1->Height() / 2.0f, 0);
	//b1 = D3DXVECTOR3(0, -collider1->Height() / 2.0f, 0);
	//a2 = D3DXVECTOR3(0,  collider2->Height() / 2.0f, 0);
	//b2 = D3DXVECTOR3(0, -collider2->Height() / 2.0f, 0);

	D3DXVec3TransformCoord(&a1, &a1, world1);
	D3DXVec3TransformCoord(&b1, &b1, world1);
	D3DXVec3TransformCoord(&a2, &a2, world2);
	D3DXVec3TransformCoord(&b2, &b2, world2);

	D3DXMATRIX mat1, mat2;
	mat1 = collider1->World() * (*world1);
	mat2 = collider2->World() * (*world2);

	D3DXVECTOR3 S1, T1;
	D3DXVECTOR3 S2, T2;
	D3DXQUATERNION R1, R2;
	D3DXMatrixDecompose(&S1, &R1, &T1, &mat1);
	D3DXMatrixDecompose(&S2, &R2, &T2, &mat2);

	//D3DXVec3TransformCoord(&a1, &a1, &mat1);
	//D3DXVec3TransformCoord(&b1, &b1, &mat1);
	//D3DXVec3TransformCoord(&a2, &a2, &mat2);
	//D3DXVec3TransformCoord(&b2, &b2, &mat2);

	float dist2 = Math::ClosestPtSegmentSegment(a1, b1, a2, b2, s, t, c1, c2);
	float radius = collider1->Radius() * S1.x + collider2->Radius() * S2.x;

	bool result = dist2 <= (radius * radius);

	return result;
}



#ifndef EFFICIENT_RAY_COLLISION
bool Collision::IsOverlapRaySphere(Ray * ray, SphereCollider * collider, OUT float* tMin, OUT float* tMax)
{
	D3DXVECTOR3 origin, direction;
	ray->Get(&origin, &direction);
	float radius = collider->Radius();
	radius *= collider->Scale().x;
	D3DXVECTOR3 center = collider->Position();

	D3DXVECTOR3 CO = origin - center;

	float a = D3DXVec3Dot(&direction, &direction);
	float b = 2.0f * D3DXVec3Dot(&CO, &direction);
	float c = D3DXVec3Dot(&CO, &CO) - (radius * radius);

	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
		return false;

	*tMin = (-b + sqrtf(discriminant)) / (2.0f * a);
	*tMax = (-b + sqrtf(discriminant)) / (2.0f * a);

	if (*tMin > *tMax) {
		float temp = *tMin;
		*tMin = *tMax;
		*tMax = temp;
	}

	return true;
}
bool Collision::ISOverlapRayCapsule(Ray * ray, CapsuleCollider * collider, OUT D3DXVECTOR3 * p1, OUT D3DXVECTOR3 * p2, OUT D3DXVECTOR3 * n1, OUT D3DXVECTOR3 * n2)
{
	/*
	t' = (t * dot(AB, d) + dot(AB, AO)) / dot(AB, AB);
	t' = t * m + n where
	m = dot(AB, d) / dot(AB, AB) and
	n = dot(AB, AO) / dot(AB, AB)
	*/

	D3DXVECTOR3 origin, direction;
	ray->Get(&origin, &direction);
	D3DXVECTOR3 A, B;
	float radius;
	collider->Get(&A, &B, &radius);

	D3DXVECTOR3 AB = B - A;
	D3DXVECTOR3 AO = origin - A;

	float AB_dot_d = D3DXVec3Dot(&AB, &direction);
	float AB_dot_AO = D3DXVec3Dot(&AB, &AO);
	float AB_dot_AB = D3DXVec3Dot(&AB, &AB);

	float m = AB_dot_d / AB_dot_AB;
	float n = AB_dot_AO / AB_dot_AB;

	/*
	dot(Q, Q)*t^2 + 2*dot(Q, R)*t + (dot(R, R) - r^2) = 0
	where
	Q = d - AB * m
	R = AO - AB * n
	*/
	D3DXVECTOR3 Q = direction - (AB * m);
	D3DXVECTOR3 R = AO - (AB * n);

	float a = D3DXVec3Dot(&Q, &Q);
	float b = 2.0f * D3DXVec3Dot(&Q, &R);
	float c = D3DXVec3Dot(&R, &R) - (radius * radius);

	if (a <= D3DX_16F_EPSILON)
	{
		/*
		Special case : AB and ray direction are parallel.
		If there is an intersection it will be on the end spheres...

		Q = d - AB * m =>
		Q = d - AB * (|AB| * |d| * cos(AB, d) / |AB|^2) => |d| == 1.0
		Q = d - AB * (|AB| * cos(AB,d) / |AB|^2) =>
		Q = d - AB * cos(AB, d) / |AB| =?
		Q = d - unit(AB) * cos(AB, d)

		|Q| == 0 means Q = (0, 0, 0) or d = unit(AB) * cos(AB,d)
		both d and unit(AB) are unit vectors, so cos(AB, d) = 1 => AB and d are parallel.
		*/

		SphereCollider sphereA(1,false), sphereB(1,false);
		sphereA.Position(A);
		sphereA.Radius(radius);
		sphereB.Position(B);
		sphereB.Radius(radius);

		float atMin, atMax, btMin, btMax;
		if (!Collision::IsOverlapRaySphere(ray, &sphereA, &atMin, &atMax) ||
			!Collision::IsOverlapRaySphere(ray, &sphereB, &btMin, &btMax))
		{
			// No intersection with ont of the spheres means no intersection at all...
			return false;
		}

		if (atMin < btMin)
		{
			*p1 = origin + (direction * atMin);
			*n1 = *p1 - A;
			D3DXVec3Normalize(n1, n1);
		}
		else
		{
			*p1 = origin + (direction * btMin);
			*n1 = *p1 - B;
			D3DXVec3Normalize(n1, n1);
		}

		if (atMax > btMax)
		{
			*p2 = origin + (direction * atMax);
			*n2 = *p2 - A;
			D3DXVec3Normalize(n2, n2);
		}
		else
		{
			*p2 = origin + (direction * btMax);
			*n2 = *p2 - B;
			D3DXVec3Normalize(n2, n2);
		}

		return true;
	}

	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
	{
		// The ray doesn't hit hte infinite cylinder defined by (A, B).
		// No intersection.
		return false;
	}

	float tMin = (-b + sqrtf(discriminant)) / (2.0f * a);
	float tMax = (-b + sqrtf(discriminant)) / (2.0f * a);

	if (tMin > tMax)
	{
		float temp = tMin;
		tMin = tMax;
		tMax = temp;
	}

	// Now check to see if K1 and K2 are inside the line segement defined by A,B
	float t_k1 = tMin * m + n;
	if (t_k1 <= D3DX_16F_EPSILON)
	{
		// On sphere (A, r)
		SphereCollider s(1, false);
		s.Position(A);
		s.Radius(radius);

		float stMin, stMax;
		if (Collision::IsOverlapRaySphere(ray, &s, &stMin, &stMax))
		{
			*p1 = origin + (direction * stMin);
			*n1 = *p1 - A;
			D3DXVec3Normalize(n1, n1);
		}
		else
			return false;
	}
	else if (t_k1 > 1.0f)
	{
		// On Sphere (B, r)...
		SphereCollider s(1, false);
		s.Position(B);
		s.Radius(radius);

		float stMin, stMax;
		if (Collision::IsOverlapRaySphere(ray, &s, &stMin, &stMax))
		{
			*p1 = origin + (direction * stMin);
			*n1 = *p1 - B;
			D3DXVec3Normalize(n1, n1);
		}
		else
			return false;
	}
	else
	{
		// On the cylinder...
		*p1 = origin + (direction * tMin);

		D3DXVECTOR3 k1 = A + AB * t_k1;
		*n1 = *p1 - k1;
		D3DXVec3Normalize(n1, n1);
	}

	float t_k2 = tMax * m + n;
	if (t_k2 <= D3DX_16F_EPSILON)
	{
		// On sphere (A, r)...
		SphereCollider s(1, false);
		s.Position(A);
		s.Radius(radius);

		float stMin, stMax;
		if (Collision::IsOverlapRaySphere(ray, &s, &stMin, &stMax))
		{
			*p2 = origin + (direction * stMax);
			*n2 = *p2 - A;
			D3DXVec3Normalize(n2, n2);
		}
		else
			return false;
	}
	else if (t_k2 > 1.0f)
	{
		// On Sphere (B, r)...
		SphereCollider s(1, false);
		s.Position(B);
		s.Radius(radius);

		float stMin, stMax;
		if (Collision::IsOverlapRaySphere(ray, &s, &stMin, &stMax))
		{
			*p2 = origin + (direction * stMax);
			*n2 = *p2 - B;
			D3DXVec3Normalize(n2, n2);
		}
		else
			return false;
	}
	else
	{
		// On the cylinder...
		*p2 = origin + (direction * tMax);

		D3DXVECTOR3 k2 = A + AB * t_k2;
		*n2 = *p2 - k2;
		D3DXVec3Normalize(n2, n2);
	}

	return true;
}
#else
bool Collision::IsOverlapRaySphere(Ray * ray, SphereCollider * collider)
{
	D3DXVECTOR3 origin, direction;
	ray->Get(&origin, &direction);
	D3DXVec3Normalize(&direction, &direction);

	D3DXVECTOR3 center = collider->Position();
	float radius = collider->Radius() * collider->Scale().x;

	D3DXVECTOR3 Q = center - origin;
	float v = D3DXVec3Dot(&Q, &direction);
	float d = (radius * radius) - (D3DXVec3LengthSq(&Q) - v * v);

	return d >= D3DX_16F_EPSILON;
}
bool Collision::IsOverlayRayCapsule(Ray * ray, CapsuleCollider * collider)
{
	D3DXVECTOR3 origin, direction;
	ray->Get(&origin, &direction);
	D3DXVec3Normalize(&direction, &direction);

	D3DXVECTOR3 min, max;
	float radius;
	collider->Get(&min, &max, &radius);

	D3DXVECTOR3 delta = max - min;
	D3DXVec3Normalize(&delta, &delta);
	D3DXVECTOR3 center = min;

	SphereCollider sphere(1.0f, false);
	sphere.Position(min);
	sphere.Radius(radius);

	bool result = Collision::IsOverlapRaySphere(ray, &sphere);

	float distance = floor(D3DXVec3Length(&(max - min)));

	for (int i = 1; i < distance; i++)
	{
		sphere.Position(min + delta * (float)i);
		result |= Collision::IsOverlapRaySphere(ray, &sphere);
	}

	sphere.Position(max);
	result |= Collision::IsOverlapRaySphere(ray, &sphere);

	return result;
}
#endif // !EFFICIENT_RAY_COLLISION

