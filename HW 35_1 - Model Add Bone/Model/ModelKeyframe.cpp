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

void ModelKeyframe::GetTransform(int frame, OUT D3DXVECTOR3 * S, OUT D3DXQUATERNION * R, OUT D3DXVECTOR3 * T)
{
	*S = transforms[frame].S;
	*R = transforms[frame].R;
	*T = transforms[frame].T;
}

void ModelKeyframe::SetTransform(int frame, D3DXVECTOR3 & S, D3DXQUATERNION & R, D3DXVECTOR3 & T)
{
	transforms[frame].S = S;
	transforms[frame].R = R;
	transforms[frame].T = T;
}

void ModelKeyframe::GetTime(int frame, OUT float * time)
{
	*time = transforms[frame].Time;
}

UINT ModelKeyframe::GetKeyframeIndex(float time)
{
	UINT start = 0, end = frameCount - 1;

	// 플레이가 종료된 상황
	if (time >= transforms[end].Time)
		return end;

	// time이 어느 프레임에 속해 잇는지 찾기
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
	// index1 2 현재 프레임 다음 프레임 인덱스

	index1 = index2 = 0;
	interpolatedTime = 0.0f;

	// 플레이될 프레임 인덱스
	index1 = GetKeyframeIndex(time);

	if (bRepeat == false)
		index2 = (index1 >= frameCount - 1) ? index1 : index1 + 1;
	else 
		index2 = (index1 >= frameCount - 1) ? 0 : index1 + 1;

	// 마지막 프레임
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
	// 중심점이 맞춰져 잇어서 SRT 순으로 안해도됨
	// S R 중점이므로 바뀌어도 됨

	D3DXVECTOR3 scale;
	D3DXMATRIX S;
	D3DXVec3Lerp(&scale, &transforms[index1].S, &transforms[index2].S, t);
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
