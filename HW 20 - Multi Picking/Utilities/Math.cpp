#include "stdafx.h"
#include "Math.h"

const float Math::PI = 3.14159265f;

// ������ ���� ���Ѱ� % �Ⱦ��� ����� �Ͻ� ������ �Ҽ� ������ �׷���
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
	// ȭ�� ������ �˾Ƴ����� -> viewport�� ����
	D3DXVECTOR2 screenSize;
	screenSize.x = vp->GetWidth();
	screenSize.y = vp->GetHeight();

	// ���콺�� ��ġ�� viewport�� ����ȯ�� ���
	D3DXVECTOR2 point;

	// Inv Viewport
	{
		point.x = ((2.0f * mouse.x) / screenSize.x) - 1.0f;
		point.y = (((2.0f * mouse.y) / screenSize.y) - 1.0f) * -1.0f;
	}

	return point;
}