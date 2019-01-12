#include "stdafx.h"
#include "FbxUtility.h"
#include "FbxType.h"

//FbxAxisSystem FbxUtility::Axis = FbxAxisSystem::DirectX;
bool FbxUtility::bRightHand = false;

// Scale(1, 1, -1) 마야 일 경우 z축만 뒤집으면 됨, RotationY(180.0f)
const D3DXMATRIX FbxUtility::Negative =
{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, -1, 0,
	0, 0, 0, 1
};

D3DXVECTOR2 FbxUtility::ToVector2(FbxVector2 & vec)
{
	return D3DXVECTOR2((float)vec.mData[0], (float)vec.mData[1]);
}

D3DXVECTOR3 FbxUtility::ToVector3(FbxVector4 & vec)
{
	return D3DXVECTOR3((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2]);
}

D3DXCOLOR FbxUtility::ToColor(FbxVector4 & vec)
{
	return D3DXCOLOR((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2], 1);
}

D3DXCOLOR FbxUtility::ToColor(FbxPropertyT<FbxDouble3>& vec, FbxPropertyT<FbxDouble>& factor)
{
	FbxDouble3 color = vec;

	D3DXCOLOR result;
	result.r = (float)color.mData[0];
	result.g = (float)color.mData[1];
	result.b = (float)color.mData[2];
	result.a = (float)factor;

	return result;
}

D3DXQUATERNION FbxUtility::ToQuternion(FbxQuaternion & quat)
{
	D3DXQUATERNION result;
	result.x = (float)quat.mData[0];
	result.y = (float)quat.mData[1];
	result.z = (float)quat.mData[2];
	result.w = (float)quat.mData[3];

	return result;
}

D3DXMATRIX FbxUtility::ToMatrix(FbxAMatrix & matrix)
{
	D3DXVECTOR3 S = ToVector3(matrix.GetS());
	D3DXVECTOR3 T = ToVector3(matrix.GetT());
	D3DXQUATERNION R = ToQuternion(matrix.GetQ());

	D3DXMATRIX s, r, t, w;
	D3DXMatrixScaling(&s, S.x, S.y, S.z);
	D3DXMatrixTranslation(&t, T.x, T.y, T.z);
	D3DXMatrixRotationQuaternion(&r, &R);

	w = s * r * t;

	//if (Axis == FbxAxisSystem::MayaYUp)
	if(bRightHand == true)
		return Negative * w * Negative;

	return w;
}

D3DXVECTOR3 FbxUtility::ToPosition(FbxVector4 & vec)
{
	D3DXVECTOR3 temp = ToVector3(vec);

	//if (Axis == FbxAxisSystem::MayaYUp)
	if (bRightHand == true)
		D3DXVec3TransformCoord(&temp, &temp, &Negative);

	return temp;
}

D3DXVECTOR3 FbxUtility::ToNormal(FbxVector4 & vec)
{
	D3DXVECTOR3 temp = ToVector3(vec);

	//if (Axis == FbxAxisSystem::MayaYUp)
	if (bRightHand == true)
		D3DXVec3TransformNormal(&temp, &temp, &Negative);

	return temp;
}

string FbxUtility::GetTextureFile(FbxProperty & prop)
{
	// property 사용가능한지 체크
	if(prop.IsValid() == true)
	{
		if (prop.GetSrcObjectCount() > 0)
		{
			FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>();

			if (texture != NULL)
				// 파일의 전체 경로
				return string(texture->GetFileName());
		}
	}

	return "";
}

string FbxUtility::GetMaterialName(FbxMesh * mesh, int polygonIndex, int cpIndex)
{
	// mesh가 소속되어있는 node
	FbxNode* node = mesh->GetNode();
	if (node == NULL) return "";

	// fbx surface material의 자식
	// layer 0 맨 아래껏만씀
	// 게임에선 계층을 안씀 -> 0계층만 고정시켜씀
	FbxLayerElementMaterial * material = mesh->GetLayer(0)->GetMaterials();

	if (material == NULL) return "";

	FbxLayerElement::EMappingMode mappingMode = material->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = material->GetReferenceMode();

	int mappingIndex = -1;
	switch (mappingMode)
	{
	case FbxLayerElement::eAllSame: mappingIndex = 0; break;
	case FbxLayerElement::eByPolygon: mappingIndex = polygonIndex; break;
	case FbxLayerElement::eByControlPoint: mappingIndex = cpIndex; break;
	case FbxLayerElement::eByPolygonVertex: mappingIndex = polygonIndex * 3; break;
	default: assert(false); break;
	}


	FbxSurfaceMaterial* findMaterial = NULL;
	if (refMode == FbxLayerElement::eDirect)
	{
		if (mappingIndex < node->GetMaterialCount())
			findMaterial = node->GetMaterial(mappingIndex);
	}
	else if (refMode == FbxLayerElement::eIndexToDirect)
	{
		FbxLayerElementArrayTemplate<int>& indexArr = material->GetIndexArray();

		if (mappingIndex < indexArr.GetCount())
		{
			int tempIndex = indexArr.GetAt(mappingIndex);
			
			if (tempIndex < node->GetMaterialCount())
				findMaterial = node->GetMaterial(tempIndex);
		} // if(mappingIndex)
	} // if(refMode)

	if (findMaterial == NULL)
		return "";

	return findMaterial->GetName();
}

D3DXVECTOR2 FbxUtility::GetUv(FbxMesh * mesh, int cpIndex, int uvIndex)
{
	D3DXVECTOR2 result = D3DXVECTOR2(0, 0);
	
	FbxLayerElementUV* uv = mesh->GetLayer(0)->GetUVs();
	if (uv == NULL) return result;

	FbxLayerElement::EMappingMode mappingMode = uv->GetMappingMode();
	FbxLayerElement::EReferenceMode refMode = uv->GetReferenceMode();

	switch (mappingMode)
	{
		case FbxLayerElement::eByControlPoint:
		{
			if (refMode == FbxLayerElement::eDirect)
			{
				result.x = (float)uv->GetDirectArray().GetAt(cpIndex).mData[0];
				result.y = (float)uv->GetDirectArray().GetAt(cpIndex).mData[1];
			}
			else if (refMode = FbxLayerElement::eIndexToDirect)
			{
				int index = uv->GetIndexArray().GetAt(cpIndex);

				result.x = (float)uv->GetDirectArray().GetAt(index).mData[0];
				result.y = (float)uv->GetDirectArray().GetAt(index).mData[1];
			}
		}
		break;

		case FbxLayerElement::eByPolygonVertex:
		{
			result.x = (float)uv->GetDirectArray().GetAt(uvIndex).mData[0];
			result.y = (float)uv->GetDirectArray().GetAt(uvIndex).mData[1];
		}
		break;
	}

	//if (Axis == FbxAxisSystem::MayaYUp)
	if (bRightHand == true)
		result.y = 1.0f - result.y;

	return result;
}

