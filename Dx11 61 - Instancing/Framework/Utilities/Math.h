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

	static float Lerp(float v1, float v2, float t);

	static float LerpAngle(float a, float b, float t);
	static float NormalizeAngle(float x);

	static float FPosMod(float x, float y);

	static void QuatToPitchYawRoll(const D3DXQUATERNION& q,
		float& pitch, float& yaw, float& roll, bool bToDegree = false);

	static float ClosestPtSegmentSegment(
		const D3DXVECTOR3& p1, const D3DXVECTOR3& q1, const D3DXVECTOR3& p2, const D3DXVECTOR3& q2,
		float& s, float& t, D3DXVECTOR3& c1, D3DXVECTOR3& c2);

	static D3DXQUATERNION LookAt(D3DXVECTOR3 & forward, D3DXVECTOR3 & up);

	static void toEulerAngle(const D3DXQUATERNION & q, float & pitch, float & yaw, float & roll);

	static void toEulerAngle(const D3DXQUATERNION & q, D3DXVECTOR3 & out);

	static D3DXVECTOR3 MultiplyQuatVec3(const D3DXQUATERNION& quat, const D3DXVECTOR3& vec);

	static D3DXVECTOR3 D3DXVec3Rotate(const D3DXVECTOR3 *pV, const D3DXQUATERNION *pQ);

	static D3DXVECTOR3 RotateVectorByQuaternion(const D3DXVECTOR3& v, const D3DXQUATERNION& q);

	static void D3DXVec3OrthoNormalize(D3DXVECTOR3 * normal, D3DXVECTOR3 * tangent);

	static float SquaredNorm(D3DXVECTOR3 & vec);

	static float DiagonalDistance2(Point start, Point goal);
};