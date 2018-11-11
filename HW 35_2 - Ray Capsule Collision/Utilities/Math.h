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

	static void LerpMatrix(OUT D3DXMATRIX& out, D3DXMATRIX& m1, D3DXMATRIX& m2, float amount);

	static void QuatToPitchYawRoll(const D3DXQUATERNION& q,
		float& pitch, float& yaw, float& roll, bool bToDegree = false);

	static float ClosestPtSegmentSegment(
		const D3DXVECTOR3& p1, const D3DXVECTOR3& q1, const D3DXVECTOR3& p2, const D3DXVECTOR3& q2,
		float& s, float& t, D3DXVECTOR3& c1, D3DXVECTOR3& c2);
};