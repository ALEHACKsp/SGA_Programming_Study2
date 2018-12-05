#include "stdafx.h"
#include "Math.h"

const float Math::PI = 3.14159265f;

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

// 내가 있는 위치 origin 볼 위치 target up 
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

int Math::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}