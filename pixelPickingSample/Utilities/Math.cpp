#include "stdafx.h"
#include "Math.h"

const float Math::PI = 3.14159265f;
const float Math::EPSILON = 0.000001f;

float Math::Modulo(float val1, float val2)
{
	while (val1 - val2 >= 0)
		val1 -= val2;

	return val1;
}

float Math::ToRadian(float degree)
{
	return degree * 0.01745329f;
}

float Math::ToDegree(float radian)
{
	return radian * 57.29577957f;
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

float Math::GetAngle(D3DXVECTOR3& a, D3DXVECTOR3& b)
{

	float cosAngle = acosf(D3DXVec3Dot(&a, &b) / (D3DXVec3Length(&a) * D3DXVec3Length(&b)));

	//외적의 z성분으로 방향이 결정된다.

	float angle = (a.x * b.y - a.y * b.x > 0.0f) ? cosAngle : -cosAngle;

	return angle;

}

float Math::GetDistance(D3DXVECTOR3 & a, D3DXVECTOR3 & b)
{
	D3DXVECTOR3 dir = a - b;
	return D3DXVec3Length(&dir);
}

float Math::Lerp(float f1, float f2, float t)
{
	return f1 * (1 - t) + (f2 * t);
}

void Math::LerpMatrix(OUT D3DXMATRIX & out, D3DXMATRIX & m1, D3DXMATRIX & m2, float t)
{
	out._11 = m1._11 + (m2._11 - m1._11) * t;
	out._12 = m1._12 + (m2._12 - m1._12) * t;
	out._13 = m1._13 + (m2._13 - m1._13) * t;
	out._14 = m1._14 + (m2._14 - m1._14) * t;

	out._21 = m1._21 + (m2._21 - m1._21) * t;
	out._22 = m1._22 + (m2._22 - m1._22) * t;
	out._23 = m1._23 + (m2._23 - m1._23) * t;
	out._24 = m1._24 + (m2._24 - m1._24) * t;

	out._31 = m1._31 + (m2._31 - m1._31) * t;
	out._32 = m1._32 + (m2._32 - m1._32) * t;
	out._33 = m1._33 + (m2._33 - m1._33) * t;
	out._34 = m1._34 + (m2._34 - m1._34) * t;

	out._41 = m1._41 + (m2._41 - m1._41) * t;
	out._42 = m1._42 + (m2._42 - m1._42) * t;
	out._43 = m1._43 + (m2._43 - m1._43) * t;
	out._44 = m1._44 + (m2._44 - m1._44) * t;
}

D3DXVECTOR3 Math::GetRotation(const D3DXQUATERNION& q)
{
	float pitch = 0.0f, yaw = 0.0f, roll = 0.0f;

	// pitch
	float sinr = +2.0f * (q.w * q.x + q.y * q.z);
	float cosr = +1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	pitch = atan2(sinr, cosr);

	// yaw
	float sinp = +2.0f * (q.w * q.y - q.z * q.x);
	yaw = (fabs(sinp) >= 1.0f) ? copysign(PI / 2.0f, sinp) : asin(sinp);

	// roll
	float siny = +2.0f * (q.w * q.z + q.x * q.y);
	float cosy = +1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	roll = atan2(siny, cosy);

	return D3DXVECTOR3(pitch, yaw, roll);
}

D3DXQUATERNION Math::LookAt(const D3DXVECTOR3 & origin, const D3DXVECTOR3 & target, const D3DXVECTOR3 & up)
{
	D3DXVECTOR3 f = (origin - target);
	D3DXVec3Normalize(&f, &f);

	D3DXVECTOR3 s;
	D3DXVec3Cross(&s, &up, &f);
	D3DXVec3Normalize(&s, &s);

	D3DXVECTOR3 u;
	D3DXVec3Cross(&u, &f, &s);

	float z = 1.0f + s.x + u.y + f.z;
	float fd = 2.0f * sqrtf(z);

	D3DXQUATERNION result;

	if (z > Math::EPSILON)
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

void Math::ToEulerAngle(const D3DXQUATERNION & q, float & pitch, float & yaw, float & roll)
{
	// pitch (x-axis rotation)
	float sinr = +2.0f * (q.w * q.x + q.y * q.z);
	float cosr = +1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	pitch = atan2f(sinr, cosr);

	// yaw (y-axis rotation)
	float sinp = +2.0f * (q.w * q.y - q.z * q.x);
	if (fabs(sinp) >= 1)
		yaw = (float)copysign(D3DX_PI / 2.0f, sinp); // use 90 degrees if out of range
	else
		yaw = asinf(sinp);

	// roll (z-axis rotation)
	float siny = +2.0f * (q.w * q.z + q.x * q.y);
	float cosy = +1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	roll = atan2f(siny, cosy);
}

void Math::ToEulerAngle(const D3DXQUATERNION & q, D3DXVECTOR3 & out)
{
	ToEulerAngle(q, out.x, out.y, out.z);
}

void Math::GetOrientedData(D3DXVECTOR3 axis[3], D3DXVECTOR3 & pos, D3DXMATRIX mat)
{
	axis[0].x = mat._11;	axis[0].y = mat._12;	axis[0].z = mat._13;
	axis[1].x = mat._21;	axis[1].y = mat._22;	axis[1].z = mat._23;
	axis[2].x = mat._31;	axis[2].y = mat._32;	axis[2].z = mat._33;
	pos.x = mat._41;	pos.y = mat._42;	pos.z = mat._43;

	for (size_t i = 0; i < 3; i++)
	{
		D3DXVec3Normalize(&axis[i], &axis[i]);
	}
}

bool Math::FloatEqual(float fa, float fb)
{
	return abs(fa - fb) < EPSILON;
}

int Math::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}