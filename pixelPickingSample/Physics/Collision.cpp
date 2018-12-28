#include "stdafx.h"
#include "Collision.h"
#include "Collider.h"
#include "RayCollider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "PlaneCollider.h"

CollisionCallback Dispatch[Shape::eCount][Shape::eCount] =
{
	{
		RaytoRay, RaytoSphere, RaytoBox, RaytoCapsule, RaytoPlane
	},
	{
		SpheretoRay, SpheretoSphere, SpheretoBox, SpheretoCapsule, SpheretoPlane
	},
	{
		BoxtoRay, BoxtoSphere, BoxtoBox, BoxtoCapsule, BoxtoPlane
	},
	{
		CapsuletoRay, CapsuletoSphere, CapsuletoBox, CapsuletoCapsule, CapsuletoPlane
	},
	{
		PlanetoRay, PlanetoSphere, PlanetoBox, PlanetoCapsule, PlanetoPlane
	},
};

bool RaytoRay(Collider * a, Collider * b, float *dist)
{
	return false;
}

bool RaytoSphere(Collider * a, Collider * b, float *dist)
{
	RayCollider* A = reinterpret_cast<RayCollider*>(a);
	SphereCollider* B = reinterpret_cast<SphereCollider*>(b);
	D3DXVECTOR3 direction;
	D3DXVec3Normalize(&direction, &A->GetDirection());

	D3DXVECTOR3 BtoA = A->GetPosition() - B->GetPosition();
	float BtoADotDirection = D3DXVec3Dot(&BtoA, &direction);
	float c = D3DXVec3Dot(&BtoA, &BtoA) - B->GetRadius() * B->GetRadius();
	
	// 원 밖에 있는가 (c > 0) and 예각인가 (b > 0)
	if (c > 0.0f && BtoADotDirection > 0.0f) return 0;

	// 피타고라스 정의로 검사
	float discr = BtoADotDirection * BtoADotDirection - c;
	if (discr < 0.0f) return 0;
	
	// 반직선과 구의 접점과의 거리
	float t = -BtoADotDirection - sqrt(discr);
	// 음수라면 반직선의 시작점이 구 안에 있는 것, t를 0으로
	if (t < 0.0f) t = 0.0f;
	
	// 접점 D3DXVECTOR3 point = A->Translation() + t * direction;
	if(dist != NULL)
		*dist = t;
	return 1;
}

bool RaytoBox(Collider * a, Collider * b, float *dist)
{
	RayCollider* A = reinterpret_cast<RayCollider*>(a);
	BoxCollider* B = reinterpret_cast<BoxCollider*>(b);

	D3DXVECTOR3 position = A->GetPosition();
	D3DXVECTOR3 direction = A->GetDirection();

	D3DXVECTOR3 min, max;
	B->GetBox(&min, &max);

	*dist = 0.0f; // set to -FLT_MAX to get first hit on line
	float tmax = FLT_MAX; // set to max distance ray can travel (for segment)

						  // For all three slabs
	if (abs(direction.x) < Math::EPSILON)
	{
		if (position.x < min.x || position.x > min.x)
			return false;
	}
	else
	{
		// Compute intersection t value of ray with near and far plane of slab
		float ood = 1.0f / direction.x;
		float t1 = (min.x - position.x) * ood;
		float t2 = (max.x - position.x) * ood;
		// Make t1 be intersection with near plane, t2 with far plane
		if (t1 > t2) swap(t1, t2);
		// Compute the intersection of slab intersection intervals
		if (t1 > *dist) *dist = t1;
		if (t2 > tmax) tmax = t2;
		// Exit with no collision as soon as slab intersection becomes empty
		if (*dist > tmax) return false;
	}
	if (abs(direction.y) < Math::EPSILON)
	{
		if (position.y < min.y || position.y > min.y)
			return false;
	}
	else
	{
		float ood = 1.0f / direction.y;
		float t1 = (min.y - position.y) * ood;
		float t2 = (max.y - position.y) * ood;
		
		if (t1 > t2) swap(t1, t2);
		if (t1 > *dist) *dist = t1;
		if (t2 > tmax) tmax = t2;
		if (*dist > tmax) return false;
	}
	if (abs(direction.z) < Math::EPSILON)
	{
		if (position.z < min.z || position.z > min.z)
			return false;
	}
	else
	{
		float ood = 1.0f / direction.z;
		float t1 = (min.z - position.z) * ood;
		float t2 = (max.z - position.z) * ood;

		if (t1 > t2) swap(t1, t2);
		if (t1 > *dist) *dist = t1;
		if (t2 > tmax) tmax = t2;
		if (*dist > tmax) return false;
	}
	// Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
	return true;
}

bool RaytoCapsule(Collider * aa, Collider * bb, float *dist)
{
	RayCollider* A = reinterpret_cast<RayCollider*>(aa);
	CapsuleCollider* B = reinterpret_cast<CapsuleCollider*>(bb);
	D3DXVECTOR3 direction, positionA;
	D3DXVec3Normalize(&direction, &A->GetDirection());
	positionA = A->GetPosition();
	D3DXVECTOR3 point1, point2;
	float radius;
	B->GetCapsule(&point2, &point1, &radius);
	float t;

	///////////
	{
		D3DXVECTOR3 BtoA = positionA - point1;
		float BtoADotDirection = D3DXVec3Dot(&BtoA, &direction);
		float c = D3DXVec3Dot(&BtoA, &BtoA) - radius * radius;
		float discr = BtoADotDirection * BtoADotDirection - c;
		if (discr >= 0.0f) return true;
	}
	{
		D3DXVECTOR3 BtoA = positionA - point2;
		float BtoADotDirection = D3DXVec3Dot(&BtoA, &direction);
		float c = D3DXVec3Dot(&BtoA, &BtoA) - radius * radius;
		float discr = BtoADotDirection * BtoADotDirection - c;
		if (discr >= 0.0f) return true;
	}
	///////////
	
	D3DXVECTOR3 d = point2 - point1;
	D3DXVECTOR3 m = positionA - point1;
	D3DXVECTOR3 n = direction * 800;
	float md = D3DXVec3Dot(&m, &d);
	float nd = D3DXVec3Dot(&n, &d);
	float dd = D3DXVec3Dot(&d, &d);

	// 선분이 실린더 밖에 있는가 체크
	if (md < 0.0f && md + nd < 0.0f) return 0; // point1 밖에 있다.
	if (md > dd && md + nd > dd) return 0; // point2 밖에 있다.

	float nn = D3DXVec3Dot(&n, &n);
	float mn = D3DXVec3Dot(&m, &n);
	float a = dd * nn - nd * nd;
	float k = D3DXVec3Dot(&m, &m) - radius * radius;
	float c = dd * k - md * md;
	if (abs(a) < Math::EPSILON) {
		// 실린더 라인과 평행
		if (c > 0.0f) return 0; // a and 실린더 밖에 있다
								// 실린더와 비교
		if (md < 0.0f) t = -mn / nn; // point1과 비교
		else if (md > dd) t = (nd - mn) / nn; // point2와 비교
		else t = 0.0f; // 실린더 안에 있다.
		return 1;
	}
	float b = dd * mn - nd * md;
	float discr = b * b - a * c;
	if (discr < 0.0f) return 0; // 충돌없음

	t = (-b - sqrt(discr)) / a;
	if (t < 0.0f || t > 1.0f) return 0; // Intersection lies outside segment
	if (md + t * nd < 0.0f) {
		// Intersection outside cylinder on ’p’ side
		if (nd <= 0.0f) return 0; // Segment pointing away from endcap
		t = -md / nd;
		// Keep intersection if Dot(S(t) - p, S(t) - p) <= r∧2
		return k + 2 * t * (mn + t * nn) <= 0.0f;
	}
	else if (md + t * nd > dd) {
		// Intersection outside cylinder on ’q’ side
		if (nd >= 0.0f) return 0; // Segment pointing away from endcap
		t = (dd - md) / nd;
		// Keep intersection if Dot(S(t) - q, S(t) - q) <= r∧2
		return k + dd - 2 * md + t * (2 * (mn - nd) + t * nn) <= 0.0f;
	}
	// Segment intersects cylinder between the endcaps; t is correct
	return 1;
}

bool RaytoPlane(Collider * a, Collider * b, float * dist)
{
	RayCollider* A = reinterpret_cast<RayCollider*>(a);
	PlaneCollider* B = reinterpret_cast<PlaneCollider* >(b);

	float NdotD = D3DXVec3Dot(&B->GetNormal(), &A->GetDirection());

	//평행
	if (NdotD == 0)
		return false;

	float t = (-D3DXVec3Dot(&B->GetNormal(), &A->GetPosition()) - B->GetD()) / NdotD;
	
	if (t < 0)
		return false;

	*dist = t;
	return true;
}

bool SpheretoRay(Collider * a, Collider * b, float *dist)
{
	return false;
}

bool SpheretoSphere(Collider * a, Collider * b, float *dist)
{
	SphereCollider* A = reinterpret_cast<SphereCollider*>(a);
	SphereCollider* B = reinterpret_cast<SphereCollider*>(b);

	D3DXVECTOR3 Apos, Bpos, AToB;
	float Aradius, Bradius;
	float distance;

	Apos = A->GetPosition();
	Aradius = A->GetRadius();

	Bpos = B->GetPosition();
	Bradius = B->GetRadius();

	AToB = Bpos - Apos;
	distance = D3DXVec3Length(&AToB);
	if ((Aradius + Bradius) < distance)
		return false;
	return true;
}

bool SpheretoBox(Collider * a, Collider * b, float *dist)
{
	return BoxtoSphere(b, a, dist);
}

bool SpheretoCapsule(Collider * a, Collider * b, float *dist)
{
	return false;
}

bool SpheretoPlane(Collider * a, Collider * b, float * dist)
{
	return false;
}

bool BoxtoRay(Collider * a, Collider * b, float *dist)
{
	return false;
}

bool BoxtoSphere(Collider * a, Collider * b, float *dist)
{
	BoxCollider* A = reinterpret_cast<BoxCollider*>(a);
	SphereCollider* B = reinterpret_cast<SphereCollider*>(b);

	D3DXVECTOR3 Apos, Bpos, AToB;
	D3DXVECTOR3 Aaxis[3];
	float AaxisLen[3], Bradius;
	float r[3];

	{
		D3DXMATRIX Amat, Bmat;
		Amat = A->GetWorld();

		Math::GetOrientedData(Aaxis, Apos, Amat);

		D3DXVECTOR3 Ascale;
		Ascale = A->GetScale();

		AaxisLen[0] = Ascale.x / 2.0f;	AaxisLen[1] = Ascale.y / 2.0f;	AaxisLen[2] = Ascale.z / 2.0f;

		Bmat = B->GetWorld();
		Bpos = D3DXVECTOR3(Bmat._41, Bmat._42, Bmat._43);
		Bradius = B->GetRadius();
	}

	AToB = Bpos - Apos;

	for (size_t i = 0; i < 3; i++)
	{
		r[i] = D3DXVec3Dot(&AToB, &Aaxis[i]);

		if (-(AaxisLen[i] - Bradius) > r[i])
			return false;
		if ((AaxisLen[i] - Bradius) < r[i])
			return false;
	}

	return true;
}

bool BoxtoBox(Collider * a, Collider * b, float *dist)
{
	BoxCollider* A = reinterpret_cast<BoxCollider*>(a);
	BoxCollider* B = reinterpret_cast<BoxCollider*>(b);

	double c[3][3];
	double absC[3][3];
	double d[3];
	double r0, r1, r;
	int i;
	const double cutoff = 0.999999;
	bool existsParallelPair = false;

	D3DXVECTOR3 Aaxis[3], Baxis[3];
	double AaxisLen[3], BaxisLen[3];
	D3DXVECTOR3 APos, BPos;

	{
		D3DXMATRIX Amat, Bmat;
		Amat = A->GetWorld();
		Bmat = B->GetWorld();

		Math::GetOrientedData(Aaxis, APos, Amat);
		Math::GetOrientedData(Baxis, BPos, Bmat);

		D3DXVECTOR3 Ascale, Bscale;
		Ascale = A->GetScale();
		Bscale = B->GetScale();

		AaxisLen[0] = Ascale.x / 2.0f;	AaxisLen[1] = Ascale.y / 2.0f;	AaxisLen[2] = Ascale.z / 2.0f;
		BaxisLen[0] = Bscale.x / 2.0f;	BaxisLen[1] = Bscale.y / 2.0f;	BaxisLen[2] = Bscale.z / 2.0f;
	}

	D3DXVECTOR3 diff = APos - BPos;


	for (i = 0; i < 3; ++i)
	{
		c[0][i] = D3DXVec3Dot(&Aaxis[0], &Baxis[i]);
		absC[0][i] = abs(c[0][i]);
		if (absC[0][i] > cutoff)
			existsParallelPair = true;
	}


	d[0] = D3DXVec3Dot(&diff, &Aaxis[0]);
	r = abs(d[0]);
	r0 = AaxisLen[0];
	r1 = BaxisLen[0] * absC[0][0] + BaxisLen[1] * absC[0][1] + BaxisLen[2] * absC[0][2];
	if (r > r0 + r1)
	{
		return false;
	}
	for (i = 0; i < 3; ++i)
	{
		c[1][i] = D3DXVec3Dot(&Aaxis[1], &Baxis[i]);
		absC[1][i] = abs(c[1][i]);
		if (absC[1][i] > cutoff)
			existsParallelPair = true;
	}
	d[1] = D3DXVec3Dot(&diff, &Aaxis[1]);
	r = abs(d[1]);
	r0 = AaxisLen[1];
	r1 = BaxisLen[0] * absC[1][0] + BaxisLen[1] * absC[1][1] + BaxisLen[2] * absC[1][2];
	if (r > r0 + r1)
	{
		return false;
	}
	for (i = 0; i < 3; ++i)
	{
		c[2][i] = D3DXVec3Dot(&Aaxis[2], &Baxis[i]);
		absC[2][i] = abs(c[2][i]);
		if (absC[2][i] > cutoff)
			existsParallelPair = true;
	}
	d[2] = D3DXVec3Dot(&diff, &Aaxis[2]);
	r = abs(d[2]);
	r0 = AaxisLen[2];
	r1 = BaxisLen[0] * absC[2][0] + BaxisLen[1] * absC[2][1] + BaxisLen[2] * absC[2][2];
	if (r > r0 + r1)
	{
		return false;
	}
	r = abs(D3DXVec3Dot(&diff, &Baxis[0]));
	r0 = AaxisLen[0] * absC[0][0] + AaxisLen[1] * absC[1][0] + AaxisLen[2] * absC[2][0];
	r1 = BaxisLen[0];
	if (r > r0 + r1)
	{
		return false;
	}


	r = abs(D3DXVec3Dot(&diff, &Baxis[1]));
	r0 = AaxisLen[0] * absC[0][1] + AaxisLen[1] * absC[1][1] + AaxisLen[2] * absC[2][1];
	r1 = BaxisLen[1];
	if (r > r0 + r1)
	{
		return false;
	}


	r = abs(D3DXVec3Dot(&diff, &Baxis[2]));
	r0 = AaxisLen[0] * absC[0][2] + AaxisLen[1] * absC[1][2] + AaxisLen[2] * absC[2][2];
	r1 = BaxisLen[2];
	if (r > r0 + r1)
	{
		return false;
	}


	if (existsParallelPair == true)
	{
		return true;
	}


	r = abs(d[2] * c[1][0] - d[1] * c[2][0]);
	r0 = AaxisLen[1] * absC[2][0] + AaxisLen[2] * absC[1][0];
	r1 = BaxisLen[1] * absC[0][2] + BaxisLen[2] * absC[0][1];
	if (r > r0 + r1)
	{
		return false;
	}


	r = abs(d[2] * c[1][1] - d[1] * c[2][1]);
	r0 = AaxisLen[1] * absC[2][1] + AaxisLen[2] * absC[1][1];
	r1 = BaxisLen[0] * absC[0][2] + BaxisLen[2] * absC[0][0];
	if (r > r0 + r1)
	{
		return false;
	}


	r = abs(d[2] * c[1][2] - d[1] * c[2][2]);
	r0 = AaxisLen[1] * absC[2][2] + AaxisLen[2] * absC[1][2];
	r1 = BaxisLen[0] * absC[0][1] + BaxisLen[1] * absC[0][0];
	if (r > r0 + r1)
	{
		return false;
	}


	r = abs(d[0] * c[2][0] - d[2] * c[0][0]);
	r0 = AaxisLen[0] * absC[2][0] + AaxisLen[2] * absC[0][0];
	r1 = BaxisLen[1] * absC[1][2] + BaxisLen[2] * absC[1][1];
	if (r > r0 + r1)
	{
		return false;
	}


	r = abs(d[0] * c[2][1] - d[2] * c[0][1]);
	r0 = AaxisLen[0] * absC[2][1] + AaxisLen[2] * absC[0][1];
	r1 = BaxisLen[0] * absC[1][2] + BaxisLen[2] * absC[1][0];
	if (r > r0 + r1)
	{
		return false;
	}


	r = abs(d[0] * c[2][2] - d[2] * c[0][2]);
	r0 = AaxisLen[0] * absC[2][2] + AaxisLen[2] * absC[0][2];
	r1 = BaxisLen[0] * absC[1][1] + BaxisLen[1] * absC[1][0];
	if (r > r0 + r1)
	{
		return false;
	}


	r = abs(d[1] * c[0][0] - d[0] * c[1][0]);
	r0 = AaxisLen[0] * absC[1][0] + AaxisLen[1] * absC[0][0];
	r1 = BaxisLen[1] * absC[2][2] + BaxisLen[2] * absC[2][1];
	if (r > r0 + r1)
	{
		return false;
	}


	r = abs(d[1] * c[0][1] - d[0] * c[1][1]);
	r0 = AaxisLen[0] * absC[1][1] + AaxisLen[1] * absC[0][1];
	r1 = BaxisLen[0] * absC[2][2] + BaxisLen[2] * absC[2][0];
	if (r > r0 + r1)
	{
		return false;
	}


	r = abs(d[1] * c[0][2] - d[0] * c[1][2]);
	r0 = AaxisLen[0] * absC[1][2] + AaxisLen[1] * absC[0][2];
	r1 = BaxisLen[0] * absC[2][1] + BaxisLen[1] * absC[2][0];
	if (r > r0 + r1)
	{
		return false;
	}

	return true;
}

bool BoxtoCapsule(Collider * a, Collider * b, float *dist)
{
	return false;
}

bool BoxtoPlane(Collider * a, Collider * b, float * dist)
{
	return false;
}

bool CapsuletoRay(Collider * a, Collider * b, float *dist)
{
	return false;
}

bool CapsuletoSphere(Collider * a, Collider * b, float *dist)
{
	return false;
}

bool CapsuletoBox(Collider * a, Collider * b, float *dist)
{
	return false;
}

bool CapsuletoCapsule(Collider * a, Collider * b, float *dist)
{
	CapsuleCollider* A = reinterpret_cast<CapsuleCollider*>(a);
	CapsuleCollider* B = reinterpret_cast<CapsuleCollider*>(b);

	float r1, r2, s, t;
	D3DXVECTOR3 p1, q1, p2, q2, c1, c2;
	A->GetCapsule(&p1, &q1, &r1);
	B->GetCapsule(&p2, &q2, &r2);
	float radius = r1 + r2;
	
	D3DXVECTOR3 d1 = q1 - p1; // 캡슐1 두 포인트간의 방향벡터
	D3DXVECTOR3 d2 = q2 - p2; // 캡슐2 두 포인트간의 방향벡터
	D3DXVECTOR3 r = p1 - p2;
	float e1 = D3DXVec3Dot(&d1, &d1); // Squared length of segment S1, always nonnegative
	float e2 = D3DXVec3Dot(&d2, &d2); // Squared length of segment S2, always nonnegative
	float f = D3DXVec3Dot(&d2, &r);
	// Check if either or both segments degenerate into points
	if (e1 <= D3DX_16F_EPSILON && e2 <= D3DX_16F_EPSILON) {
		// Both segments degenerate into points
		s = t = 0.0f;
		c1 = p1;
		c2 = p2;
		return D3DXVec3Dot(&D3DXVECTOR3(c1 - c2), &D3DXVECTOR3(c1 - c2)) <= radius * radius;
	}
	if (e1 <= D3DX_16F_EPSILON) {
		// First segment degenerates into a point
		s = 0.0f;
		t = f / e2; // s = 0 => t = (b*s + f) / e = f / e
		t = Math::Clamp(t, 0.0f, 1.0f);
	}
	else {
		float c = D3DXVec3Dot(&d1, &r);
		if (e2 <= D3DX_16F_EPSILON) {
			// Second segment degenerates into a point
			t = 0.0f;
			s = Math::Clamp(-c / e1, 0.0f, 1.0f); // t = 0 => s = (b*t - c) / a = -c / a
		}
		else {
			// The general nondegenerate case starts here
			float b = D3DXVec3Dot(&d1, &d2);
			float denom = e1*e2 - b*b; // Always nonnegative
									 // If segments not parallel, compute closest point on L1 to L2 and
									 // clamp to segment S1. Else pick arbitrary s (here 0)
			if (denom != 0.0f) {
				s = Math::Clamp((b*f - c*e2) / denom, 0.0f, 1.0f);
			}
			else s = 0.0f;
			// Compute point on L2 closest to S1(s) using
			// t = Dot((P1 + D1*s) - P2,D2) / Dot(D2,D2) = (b*s + f) / e
			t = (b*s + f) / e2;
			// If t in [0,1] done. Else clamp t, recompute s for the new value
			// of t using s = Dot((P2 + D2*t) - P1,D1) / Dot(D1,D1)= (t*b - c) / a
			// and clamp s to [0, 1]
			if (t < 0.0f) {
				t = 0.0f;
				s = Math::Clamp(-c / e1, 0.0f, 1.0f);
			}
			else if (t > 1.0f) {
				t = 1.0f;
				s = Math::Clamp((b - c) / e1, 0.0f, 1.0f);
			}
		}
	}
	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;
	return D3DXVec3Dot(&D3DXVECTOR3(c1 - c2), &D3DXVECTOR3(c1 - c2)) <= radius * radius;
}

bool CapsuletoPlane(Collider * a, Collider * b, float * dist)
{
	return false;
}

bool PlanetoRay(Collider * a, Collider * b, float * dist)
{
	return RaytoPlane(b, a, dist);
}

bool PlanetoSphere(Collider * a, Collider * b, float * dist)
{
	return false;
}

bool PlanetoBox(Collider * a, Collider * b, float * dist)
{
	return false;
}

bool PlanetoCapsule(Collider * a, Collider * b, float * dist)
{
	return false;
}

bool PlanetoPlane(Collider * a, Collider * b, float * dist)
{
	return false;
}
