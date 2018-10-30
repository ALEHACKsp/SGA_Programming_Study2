#pragma once
class Math
{
public:
	static const float PI;

	static float Modulo(float val1, float val2);

	static float ToRadian(float degree);
	static float ToDegree(float radian);

	static int Random(int r1, int r2);
	static float Random(float r1, float r2);

	static float Clamp(float value, float min, float max);

	static void LerpMatrix(OUT D3DXMATRIX& out, D3DXMATRIX& m1, D3DXMATRIX& m2, float amount);

	D3DXQUATERNION LookAt(const D3DXVECTOR3 & origin, const D3DXVECTOR3 & target, const D3DXVECTOR3 & up);

	void toEulerAngle(const D3DXQUATERNION & q, float & pitch, float & yaw, float & roll);

	void toEulerAngle(const D3DXQUATERNION & q, D3DXVECTOR3 & out);
};