#pragma once
#include "stdafx.h"

namespace Fbx
{
	// fbx로부터 빼온 데이터 중 필요한 정보만 저장할 구조체
	struct FbxMaterial
	{
		string Name; // 저장하는건 다 string으로 할꺼 wstring binary 처리하기 귀찮음

		D3DXCOLOR Diffuse;

		string DiffuseFile;
	};
}
