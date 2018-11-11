#include "stdafx.h"
#include "Math.h"

const float Math::PI = 3.14159265f;
const float Math::EPSILON = 0.000001f;

// 나머지 연산 구한거 % 안쓰고 빼기로 하신 이유는 소수 때문에 그런듯
float Math::Modulo(float val1, float val2)
{
	while (val1 - val2 >= 0)
		val1 -= val2;

	return val1;
}

float Math::ToRadian(float degree)
{
	return degree * PI / 180.0f;
}

float Math::ToDegree(float radian)
{
	return radian * 180.0f / PI;
}

float Math::Random(float r1, float r2)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = r2 - r1;
	float val = random * diff;
	
	return r1 + val;
}

float Math::Clamp(float value, float min, float max)
{
	value = value > max ? max : value;
	value = value < min ? min : value;

	return value;
}

void Math::LerpMatrix(OUT D3DXMATRIX & out, D3DXMATRIX & m1, D3DXMATRIX & m2, float amount)
{
	out._11 = m1._11 + (m2._11 - m1._11) * amount;
	out._12 = m1._12 + (m2._12 - m1._12) * amount;
	out._13 = m1._13 + (m2._13 - m1._13) * amount;
	out._14 = m1._14 + (m2._14 - m1._14) * amount;

	out._21 = m1._21 + (m2._21 - m1._21) * amount;
	out._22 = m1._22 + (m2._22 - m1._22) * amount;
	out._23 = m1._23 + (m2._23 - m1._23) * amount;
	out._24 = m1._24 + (m2._24 - m1._24) * amount;

	out._31 = m1._31 + (m2._31 - m1._31) * amount;
	out._32 = m1._32 + (m2._32 - m1._32) * amount;
	out._33 = m1._33 + (m2._33 - m1._33) * amount;
	out._34 = m1._34 + (m2._34 - m1._34) * amount;

	out._41 = m1._41 + (m2._41 - m1._41) * amount;
	out._42 = m1._42 + (m2._42 - m1._42) * amount;
	out._43 = m1._43 + (m2._43 - m1._43) * amount;
	out._44 = m1._44 + (m2._44 - m1._44) * amount;
}

float Math::Lerp(float v1, float v2, float t)
{
	return v1 * (1 - t) + v2 * t;;
}

void Math::QuatToPitchYawRoll(const D3DXQUATERNION & q, float & pitch, float & yaw, float & roll, bool bToDegree)
{
	float sqw = q.w * q.w;
	float sqx = q.x * q.x;
	float sqy = q.y * q.y;
	float sqz = q.z * q.z;

	// rotation about x-axis
	pitch = asinf(2.0f * (q.w * q.x - q.y * q.z));
	// rotation about y-axis
	yaw = atan2f(2.0f * (q.x * q.z + q.w * q.y), (-sqx - sqy + sqz + sqw));
	// rotation about z-axis
	roll = atan2f(2.0f * (q.x * q.y + q.w * q.z), (-sqx + sqy - sqz + sqw));

	if (bToDegree)
	{
		pitch = Math::ToDegree(pitch);
		yaw = Math::ToDegree(yaw);
		roll = Math::ToDegree(roll);
	}
}

float Math::ClosestPtSegmentSegment(const D3DXVECTOR3 & p1, const D3DXVECTOR3 & q1, const D3DXVECTOR3 & p2, const D3DXVECTOR3 & q2, float & s, float & t, D3DXVECTOR3 & c1, D3DXVECTOR3 & c2)
{
	D3DXVECTOR3 d1 = q1 - p1; // Direction segment s1
	D3DXVECTOR3 d2 = q2 - p2; // Direction segment s2
	D3DXVECTOR3 r = p1 - p2;
	float a = D3DXVec3Dot(&d1, &d1); // Squared length of segment s1
	float e = D3DXVec3Dot(&d2, &d2); // Squared length of segment s2
	float f = D3DXVec3Dot(&d2, &r);

	// degenerate 퇴화시키다
	// Check if either or both segments degenerate into points
	if (a <= Math::EPSILON && e <= Math::EPSILON)
	{
		// Both segements degenerate into points
		s = t = 0.0f;
		c1 = p1;
		c2 = p2;

		return D3DXVec3Dot(&(c1 - c2), &(c1 - c2));
	}
	if (a <= Math::EPSILON)
	{
		// First segment degenerates into a point
		s = 0.0f;
		t = f / e; // s = 0 => (b*s + f) / e = f / e
		t = Math::Clamp(t, 0.0f, 1.0f);
	}
	else
	{
		float c = D3DXVec3Dot(&d1, &r);
		if (e <= Math::EPSILON)
		{
			// Second segement degenrates into a point
			t = 0.0f;
			s = Math::Clamp(-c / a, 0.0f, 1.0f); // t = 0 => s = (b*t - c) / a = -c / a
		}
		else
		{
			// The general nondegenerate case starts here
			float b = D3DXVec3Dot(&d1, &d2);
			float denom = a*e - b*b; // Always nonnegative

			// If segments not parallel, compute closest point on L1 to L2 and
			// clamp to segment S1, Else pick arbitrary s (here 0)
			if (denom != 0.0f)
				s = Math::Clamp((b*f - c*e) / denom, 0.0f, 1.0f);
			else
				s = 0.0f;

			// Compute point on L2 closest to S1(s) using
			// t = Dot((P1 + D1*s - P2,D2) / Dot(D2,D2) = (b*s + f) /e
			t = (b*s + f) / e;

			// If t in [0,1] done. Else clamp t, recompute s for the new value
			// of t using s = Dot((P2 + D2*t) - P1,D1) / Dot(D1,D1) = (t*b - c) / a
			// and clamp s to [0, 1]
			if (t < 0.0f) {
				t = 0.0f;
				s = Math::Clamp(-c / a, 0.0f, 1.0f);
			} 
			else if (t > 1.0f) {
				t = 1.0f;
				s = Math::Clamp((b - c) / a, 0.0f, 1.0f);
			}
		}
	}

	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;

	return D3DXVec3Dot(&(c1 - c2), &(c1 - c2));
}

D3DXQUATERNION Math::LookAt(D3DXVECTOR3 & forward, D3DXVECTOR3 & up)
{
	//Math::D3DXVec3OrthoNormalize(&forward, &up);

	D3DXVECTOR3 vector = forward;
	D3DXVec3Normalize(&vector, &vector);
	D3DXVECTOR3 vector2;
	D3DXVec3Cross(&vector2, &up, &vector);
	D3DXVec3Normalize(&vector2, &vector2);
	D3DXVECTOR3 vector3;
	D3DXVec3Cross(&vector3, &vector, &vector2);

	float m00 = vector2.x;
	float m01 = vector2.y;
	float m02 = vector2.z;
	float m10 = vector3.x;
	float m11 = vector3.y;
	float m12 = vector3.z;
	float m20 = vector.x;
	float m21 = vector.y;
	float m22 = vector.z;

	float num8 = (m00 + m11) + m22;

	D3DXQUATERNION quat;
	if (num8 > 0)
	{
		float num = (float)sqrt(num8 + 1.0f);
		quat.w = num * 0.5f;
		num = 0.5f / num;
		quat.x = (m12 - m21) * num;
		quat.y = (m20 - m02) * num;
		quat.z = (m01 - m10) * num;

		return quat;
	}

	if ((m00 >= m11) && (m00 >= m22))
	{
		float num7 = (float)sqrt(((1.0f + m00) - m11) - m22);
		float num4 = 0.5f / num7;
		
		quat.x = 0.5f * num7;
		quat.y = (m01 + m10) * num4;
		quat.z = (m02 + m20) * num4;
		quat.w = (m12 - m21) * num4;
		
		return quat;
	}

	if (m11 > m22)
	{
		float num6 = (float)sqrtf(((1.0f + m11) - m00) - m22);
		float num3 = 0.5f / num6;
		quat.x = (m10 + m01) * num3;
		quat.y = 0.5f * num6;
		quat.z = (m21 + m12) * num3;
		quat.w = (m20 - m02) * num3;
		return quat;
	}

	float num5 = (float)sqrtf(((1.0f + m22) - m00) - m11);
	float num2 = 0.5f / num5;
	quat.x = (m20 + m02) * num2;
	quat.y = (m21 + m12) * num2;
	quat.z = 0.5f * num5;
	quat.w = (m01 - m10) * num2;

	return quat;
}

int Math::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}

// 내가 있는 위치 origin 볼 위치 target up 
D3DXQUATERNION Math::LookAt(const D3DXVECTOR3 & origin, const D3DXVECTOR3 & target, const D3DXVECTOR3 & targetUp)
{

	{
		D3DXVECTOR3 f = (target - origin);
		D3DXVec3Normalize(&f, &f);
		
		D3DXVECTOR3 s;
		D3DXVec3Cross(&s, &targetUp, &f);
		D3DXVec3Normalize(&s, &s);
		
		D3DXVECTOR3 u;
		D3DXVec3Cross(&u, &f, &s);
		D3DXVec3Normalize(&u, &u);
		
		float z = 1.0f + s.x + u.y + f.z;
		float fd = 2.0f * sqrtf(z);
		
		D3DXQUATERNION result;
		
		if (z > D3DX_16F_EPSILON)
		{
			result.w = 0.25f * fd;
			result.x = (f.y - u.z) / fd;
			result.y = (s.z - f.x) / fd;
			result.z = (u.x - s.y) / fd;
		}
		else if (s.x > u.y && s.x > f.z)
		{
			fd = 2.0f * sqrtf(1.0f + s.x - u.y - f.z);
			result.w = (f.y - u.z) / fd;
			result.x = 0.25f * fd;
			result.y = (u.x + s.y) / fd;
			result.z = (s.z + f.x) / fd;
		}
		else if (u.y > f.z)
		{
			fd = 2.0f * sqrtf(1.0f + u.y - s.x - f.z);
			result.w = (s.z - f.x) / fd;
			result.x = (u.x - s.y) / fd;
			result.y = 0.25f * fd;
			result.z = (f.y + u.z) / fd;
		}
		else
		{
			fd = 2.0f * sqrtf(1.0f + f.z - s.x - u.y);
			result.w = (u.x - s.y) / fd;
			result.x = (s.z + f.x) / fd;
			result.y = (f.y + u.z) / fd;
			result.z = 0.25f * fd;
		}
		
		return result;
	}

	//{
	//	D3DXVECTOR3 forward = target - origin;
	//	D3DXVec3Normalize(&forward, &forward);
	//
	//	D3DXVECTOR3 right;
	//	D3DXVec3Cross(&right, &targetUp, &forward);

	//	D3DXVECTOR3 up;
	//	D3DXVec3Cross(&up, &forward, &right);

	//	// 여기 먼가 실수 있음
	//	//up = targetUp;
	//	//Math::D3DXVec3OrthoNormalize(&up, &forward);
	//	//D3DXVec3Cross(&right, &up, &forward);

	//	D3DXQUATERNION ret;
	//
	//	ret.w = sqrtf(1.0f + right.x + up.y + forward.z) * 0.5f;
	//
	//	float w4_recip = 1.0f / (4.0f * ret.w);
	//
	//	ret.x = (forward.y - up.z) * w4_recip;
	//
	//	ret.y = (right.z - forward.x) * w4_recip;
	//
	//	ret.z = (up.x - right.y) * w4_recip;
	//
	//	return ret;
	//}
}

D3DXQUATERNION Math::LookAt(const D3DXVECTOR3 & source, const D3DXVECTOR3 & dest, const D3DXVECTOR3 & targetForward, const D3DXVECTOR3 & targetUp)
{
	{
		D3DXVECTOR3 forward = dest - source;
		D3DXVec3Normalize(&forward, &forward);

		D3DXVECTOR3 right;
		D3DXVec3Cross(&right, &targetUp, &forward);

		D3DXVECTOR3 up;
		if (D3DXVec3Dot(&forward, &targetForward) >= 0)
			D3DXVec3Cross(&up, &forward, &right);
		// 뒤집힌 경우 처리
		else {
			right *= -1;
			forward *= -1;
			D3DXVec3Cross(&up, &right, &forward);
		}

		// 여기 먼가 실수 있음
		//up = targetUp;
		//Math::D3DXVec3OrthoNormalize(&up, &forward);
		//D3DXVec3Cross(&right, &up, &forward);

		D3DXQUATERNION ret;

		ret.w = sqrtf(1.0f + right.x + up.y + forward.z) * 0.5f;

		float w4_recip = 1.0f / (4.0f * ret.w);

		ret.x = (forward.y - up.z) * w4_recip;

		ret.y = (right.z - forward.x) * w4_recip;

		ret.z = (up.x - right.y) * w4_recip;

		return ret;
	}
	
	// 이 방법 문제 있음
	{
		//D3DXVECTOR3 toVec = dest - source;
		//D3DXVec3Normalize(&toVec, &toVec);

		//// Compute rotation axis
		//D3DXVECTOR3 rotAxis;
		//D3DXVec3Cross(&rotAxis, &toVec, &forward);
		//D3DXVec3Normalize(&rotAxis, &rotAxis);

		//if (Math::SquaredNorm(rotAxis) == 0)
		//	rotAxis = up;

		//float dot = D3DXVec3Dot(&D3DXVECTOR3(0, 0, 1), &toVec);
		//float ang = acosf(dot);

		//D3DXQUATERNION ret;
		//D3DXQuaternionRotationAxis(&ret, &rotAxis, ang);

		//return ret;
	}

}

void Math::toEulerAngle(const D3DXQUATERNION & q, float & pitch, float & yaw, float & roll)
{
	float sqw = q.w*q.w;
	float sqx = q.x*q.x;
	float sqy = q.y*q.y;
	float sqz = q.z*q.z;
	pitch = asinf(2.0f * (q.w*q.x - q.y*q.z)); // rotation about x-axis
	yaw = atan2f(2.0f * (q.x*q.z + q.w*q.y), (-sqx - sqy + sqz + sqw)); // rotation about y-axis
	roll = atan2f(2.0f * (q.x*q.y + q.w*q.z), (-sqx + sqy - sqz + sqw)); // rotation about z-axis
}

void Math::toEulerAngle(const D3DXQUATERNION & q, D3DXVECTOR3 & out)
{
	toEulerAngle(q, out.x, out.y, out.z);
}

D3DXVECTOR3 Math::MultiplyQuatVec3(const D3DXQUATERNION & quat, const D3DXVECTOR3 & vec)
{
	float num = quat.x * 2.0f;
	float num2 = quat.y * 2.0f;
	float num3 = quat.z * 2.0f;
	float num4 = quat.x * num;
	float num5 = quat.y * num2;
	float num6 = quat.z * num3;
	float num7 = quat.x * num2;
	float num8 = quat.x * num3;
	float num9 = quat.y * num3;
	float num10 = quat.w * num;
	float num11 = quat.w * num2;
	float num12 = quat.w * num3;
	D3DXVECTOR3 result;
	result.x = (1.0f - (num5 + num6)) * vec.x + (num7 - num12) * vec.y + (num8 + num11) * vec.z;
	result.y = (num7 + num12) * vec.x + (1.0f - (num4 + num6)) * vec.y + (num9 - num10) * vec.z;
	result.z = (num8 - num11) * vec.x + (num9 + num10) * vec.y + (1.0f - (num4 + num5)) * vec.z;

	return result;
}

// 서로 수직인 노멀벡터
void Math::D3DXVec3OrthoNormalize(D3DXVECTOR3 * normal, D3DXVECTOR3 * tangent)
{
	// way 1
	{
		D3DXVec3Normalize(normal, normal);
		D3DXVECTOR3 v;
		D3DXVec3Cross(&v, normal, tangent);
		D3DXVec3Normalize(&v, &v);
		D3DXVec3Cross(tangent, &v, normal);
	}

	// way 2
	//{
	//	D3DXVec3Normalize(normal, normal);

	//	D3DXVECTOR3 proj;
	//	D3DXVec3Scale(&proj, normal, D3DXVec3Dot(tangent, normal));
	//	D3DXVec3Subtract(tangent, tangent, &proj);
	//	D3DXVec3Normalize(tangent, tangent);
	//}
}

float Math::SquaredNorm(D3DXVECTOR3 & vec)
{
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}
