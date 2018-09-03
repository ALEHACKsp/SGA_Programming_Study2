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

D3DXVECTOR2 Math::MoustToPoint(Viewport * vp, D3DXVECTOR3 & mouse)
{
	// 화면 사이즈 알아내야함 -> viewport에 있음
	D3DXVECTOR2 screenSize;
	screenSize.x = vp->GetWidth();
	screenSize.y = vp->GetHeight();

	// 마우스의 위치가 viewport로 역변환된 결과
	D3DXVECTOR2 point;

	// Inv Viewport
	{
		point.x = ((2.0f * mouse.x) / screenSize.x) - 1.0f;
		point.y = (((2.0f * mouse.y) / screenSize.y) - 1.0f) * -1.0f;
	}

	return point;
}