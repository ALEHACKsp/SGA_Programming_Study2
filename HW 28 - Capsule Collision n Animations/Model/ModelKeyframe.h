#pragma once

class ModelKeyframe
{
public:
	friend class ModelClip;
	struct Transform;

	ModelKeyframe();
	~ModelKeyframe();

	D3DXMATRIX GetInterpolatedMatrix(float time, bool bRepeat);
	void GetTransform(int frame, OUT D3DXVECTOR3* S, OUT D3DXQUATERNION* R, OUT D3DXVECTOR3* T);
	void SetTransform(int frame, D3DXVECTOR3& S, D3DXQUATERNION& R, D3DXVECTOR3& T);
	void GetTime(int frame, OUT float* time);

private:
	// time은 deltaTime 값 들어올꺼
	UINT GetKeyframeIndex(float time);
	void CalcKeyframeIndex(float time, bool bRepeat,
		OUT UINT& index1, OUT UINT& index2, OUT float& interpolatedTime);

	// 최종적으로 본에 들어갈 매트릭스
	D3DXMATRIX GetInterpolatedMatrix(UINT index1, UINT index2, float t);

private:
	wstring boneName;

	float duration;
	float frameRate;
	UINT frameCount;

	vector<Transform> transforms;
	
public:

	struct Transform
	{
		float Time;

		D3DXVECTOR3 S;
		D3DXQUATERNION R;
		D3DXVECTOR3 T;
	};
};