#pragma once

class FbxUtility
{
public:
	static bool bRightHand;
	static const D3DXMATRIX Negative;

	static D3DXVECTOR2 ToVector2(FbxVector2& vec);
	// 위치때문에 vector4 쓴거 버리고 쓰면됨
	static D3DXVECTOR3 ToVector3(FbxVector4& vec);
	static D3DXCOLOR ToColor(FbxVector4& vec);
	static D3DXCOLOR ToColor(FbxPropertyT<FbxDouble3>& vec, FbxPropertyT<FbxDouble>& factor);
	static D3DXQUATERNION ToQuternion(FbxQuaternion& quat);
	static D3DXMATRIX ToMatrix(FbxAMatrix& matrix);

	static D3DXVECTOR3 ToPosition(FbxVector4& vec);
	static D3DXVECTOR3 ToNormal(FbxVector4& vec);

	static string GetTextureFile(FbxProperty& prop);

	static string GetMaterialName(FbxMesh* mesh, int polygonIndex, int cpIndex);
	static D3DXVECTOR2 GetUv(FbxMesh* mesh, int cpIndex, int uvIndex);

	static D3DXVECTOR4 ToVertexIndices(vector<struct FbxVertexWeightData>& weights);
	static D3DXVECTOR4 ToVertexWeights(vector<struct FbxVertexWeightData>& weights);
};
