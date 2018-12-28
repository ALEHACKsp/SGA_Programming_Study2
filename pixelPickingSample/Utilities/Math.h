#pragma once
class Math
{
public:
	static const float PI;
	static const float EPSILON;

	static float Modulo(float val1, float val2);

	static float ToRadian(float degree);
	static float ToDegree(float radian);

	static int Random(int r1, int r2);
	static float Random(float r1, float r2);

	static float Clamp(float value, float min, float max);

	static float GetAngle(D3DXVECTOR3& a, D3DXVECTOR3& b);
	static float GetDistance(D3DXVECTOR3& a, D3DXVECTOR3& b);

	static float Lerp(float f1, float f2, float t);
	static void LerpMatrix(OUT D3DXMATRIX& out, D3DXMATRIX& m1, D3DXMATRIX& m2, float t);

	static D3DXVECTOR3 GetRotation(const D3DXQUATERNION& q);


	static D3DXQUATERNION LookAt(const D3DXVECTOR3& origin, const D3DXVECTOR3& target, const D3DXVECTOR3& up);

	static void ToEulerAngle(const D3DXQUATERNION& q, float& pitch, float& yaw, float& roll);
	static void ToEulerAngle(const D3DXQUATERNION& q, D3DXVECTOR3& out);

	static void GetOrientedData(D3DXVECTOR3 axis[3], D3DXVECTOR3& pos, D3DXMATRIX mat);

	static bool FloatEqual(float fa, float fb);
};