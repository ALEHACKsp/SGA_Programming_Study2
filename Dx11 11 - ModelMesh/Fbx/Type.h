#pragma once
#include "stdafx.h"

namespace Fbx
{
	// fbx�κ��� ���� ������ �� �ʿ��� ������ ������ ����ü
	struct FbxMaterial
	{
		string Name; // �����ϴ°� �� string���� �Ҳ� wstring binary ó���ϱ� ������

		D3DXCOLOR Diffuse;

		string DiffuseFile;
	};
}
