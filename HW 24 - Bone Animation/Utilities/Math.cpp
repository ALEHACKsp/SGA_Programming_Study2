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

int Math::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}