#include "stdafx.h"
#include "ModelKeyframe.h"

ModelKeyframe::ModelKeyframe()
{

}

ModelKeyframe::~ModelKeyframe()
{

}

D3DXMATRIX ModelKeyframe::GetInterpolatedMatrix(float time, bool bRepeat)
{
	UINT index1 = 0, index2 = 0;
	float interpolatedTime = 0.0f;

	CalcKeyframeIndex(time, bRepeat, index1, index2, interpolatedTime);

	return GetInterpolatedMatrix(index1, index2, interpolatedTime);
}

void ModelKeyframe::SetBoneTransforms(D3DXMATRIX& transform, float time, UINT interpolateFrameCount)
{
	D3DXVECTOR3 scale, translation;
	D3DXQUATERNION rotation;
	
	int curIndex = GetKeyframeIndex(time);
	D3DXMatrixDecompose(&scale, &rotation, &translation, &transform);
	transforms[curIndex].S = scale;
	transforms[curIndex].R = rotation;
	transforms[curIndex].T = translation;
	
	int interpolateStart = curIndex - interpolateFrameCount;
	int interpolateEnd = curIndex + interpolateFrameCount;
	
	while (interpolateStart < 0)
		interpolateStart += frameCount;
	while (interpolateEnd >= (int)frameCount)
		interpolateEnd -= frameCount;
	
	for (UINT i = 1; i < interpolateFrameCount; ++i)
	{
		int index1 = curIndex - i;
		while (index1 < 0)
			index1 += frameCount;
		int index2 = curIndex + i;
		while (index2 >= (int)frameCount)
			index2 -= frameCount;
	
		
		float time1 = 0.0f, time2 = 0.0f;
		float interpolatedTime = 0.0f;
		
		//앞에보간
		time1 = transforms[index1].Time - transforms[interpolateStart].Time;
		time2 = transforms[curIndex].Time - transforms[interpolateStart].Time;
		interpolatedTime = time1 / time2;
	
		D3DXVec3Lerp(&scale, &transforms[interpolateStart].S, &transforms[curIndex].S, interpolatedTime);
		D3DXQuaternionSlerp(&rotation, &transforms[interpolateStart].R, &transforms[curIndex].R, interpolatedTime);
		D3DXVec3Lerp(&translation, &transforms[interpolateStart].T, &transforms[curIndex].T, interpolatedTime);
	
		transforms[index1].S = scale;
		transforms[index1].R = rotation;
		transforms[index1].T = translation;
	
		//뒤에 보간
		time1 = transforms[index2].Time - transforms[curIndex].Time;
		time2 = transforms[interpolateEnd].Time - transforms[curIndex].Time;
		interpolatedTime = time1 / time2;
	
		D3DXVec3Lerp(&scale, &transforms[curIndex].S, &transforms[interpolateEnd].S, interpolatedTime);
		D3DXQuaternionSlerp(&rotation, &transforms[curIndex].R, &transforms[interpolateEnd].R, interpolatedTime);
		D3DXVec3Lerp(&translation, &transforms[curIndex].T, &transforms[interpolateEnd].T, interpolatedTime);
	
		transforms[index2].S = scale;
		transforms[index2].R = rotation;
		transforms[index2].T = translation;
	}
}

UINT ModelKeyframe::GetKeyframeIndex(float time)
{
	UINT start = 0, end = frameCount - 1;
	if (time >= transforms[end].Time)
		return end;

	do
	{
		UINT middle = (start + end) / 2;

		if (end - start <= 1)
		{
			break;
		}
		else if (transforms[middle].Time < time)
		{
			start = middle;
		}
		else if (transforms[middle].Time > time)
		{
			end = middle;
		}
		else
		{
			start = middle;

			break;
		}
	} while ((end - start) > 1);

	return start;
}

void ModelKeyframe::CalcKeyframeIndex(float time, bool bRepeat, OUT UINT & index1, OUT UINT & index2, OUT float & interpolatedTime)
{
	index1 = index2 = 0;
	interpolatedTime = 0.0f;

	index1 = GetKeyframeIndex(time);

	if (bRepeat == false)
		index2 = (index1 >= frameCount - 1) ? index1 : index1 + 1;
	else
		index2 = (index1 >= frameCount - 1) ? 0 : index1 + 1;

	if (index1 >= frameCount - 1)
	{
		index1 = index2 = frameCount - 1;

		interpolatedTime = 1.0f;
	}
	else
	{
		float time1 = time - transforms[index1].Time;
		float time2 = transforms[index2].Time - transforms[index1].Time;

		interpolatedTime = time1 / time2;
	}
}

D3DXMATRIX ModelKeyframe::GetInterpolatedMatrix(UINT index1, UINT index2, float t)
{
	D3DXVECTOR3 scale;
	D3DXVec3Lerp(&scale, &transforms[index1].S, &transforms[index2].S, t);

	D3DXMATRIX S;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);

	
	D3DXQUATERNION q;
	D3DXMATRIX R;
	D3DXQuaternionSlerp(&q, &transforms[index1].R, &transforms[index2].R, t);
	D3DXMatrixRotationQuaternion(&R, &q);

	
	D3DXVECTOR3 translation;
	D3DXMATRIX T;
	D3DXVec3Lerp(&translation, &transforms[index1].T, &transforms[index2].T, t);
	D3DXMatrixTranslation(&T, translation.x, translation.y, translation.z);

	return S * R * T;
}
