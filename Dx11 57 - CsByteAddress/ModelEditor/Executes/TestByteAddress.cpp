#include "stdafx.h"
#include "TestByteAddress.h"

#include "Fbx\FbxLoader.h"

void TestByteAddress::Initialize()
{
	for (int i = 0; i < 128; i++)
	{
		datas[i].x = i;
		datas[i].y = i * 10;
		datas[i].z = i * 100;
		datas[i].w = i * 1000;
	}

	for (int i = 0; i < 128; i++) 
	{
		datas2[i].Position = D3DXVECTOR3(i * 10, i * 100, i * 1000);
		datas2[i].Normal   = D3DXVECTOR3(i + 10, i + 100, i + 1000);
		datas2[i].Tangent  = D3DXVECTOR3(i / 10, i / 100, i / 1000);
	}

	CsResource::CreateRawBuffer(sizeof(VertexTextureNormalTangentBlend) * 128, datas2, &input);
	CsResource::CreateRawBuffer(sizeof(VertexTextureNormalTangentBlend) * 128, NULL, &output);
	
	//CreateRawBuffer(sizeof(D3DXVECTOR4) * 128, datas, &input);
	//CreateRawBuffer(sizeof(D3DXVECTOR4) * 128, NULL, &output);

	CsResource::CreateSRV(input, &inputSRV);
	CsResource::CreateUAV(output, &outputUAV);

	shader = new Shader(Shaders + L"063_CsByteAddress.fx", true);

	shader->AsShaderResource("datas")->SetResource(inputSRV);
	shader->AsUAV("result")->SetUnorderedAccessView(outputUAV);

	//shader->Dispatch(0, 0, 1, 1, 1);
	shader->Dispatch(0, 1, 1, 1, 1);
}

void TestByteAddress::Ready()
{
	dest = CsResource::CreateAndCopyBuffer(output);

	//D3DXVECTOR4 result[128];
	VertexTextureNormalTangentBlend result[128];

	//D3D11_MAPPED_SUBRESOURCE resource;
	//D3D::GetDC()->Map(dest, 0, D3D11_MAP_READ, 0, &resource);
	//{
	//	memcpy(result, resource.pData, sizeof(D3DXVECTOR4) * 128);
	//}
	//D3D::GetDC()->Unmap(dest, 0);

	D3D11_MAPPED_SUBRESOURCE resource;
	D3D::GetDC()->Map(dest, 0, D3D11_MAP_READ, 0, &resource);
	{
		memcpy(result, resource.pData, sizeof(VertexTextureNormalTangentBlend) * 128);
	}
	D3D::GetDC()->Unmap(dest, 0);

	for (int i = 0; i < 128; i++) {
		wstring temp = L" ==>";
		temp += L"\n\tPosition : " 
			+ to_wstring(result[i].Position.x) +
			L", " + to_wstring(result[i].Position.y) +
			L", " + to_wstring(result[i].Position.z) + L"\n";

		temp += L"\tNormal : "
			+ to_wstring(result[i].Normal.x) +
			L", " + to_wstring(result[i].Normal.y) +
			L", " + to_wstring(result[i].Normal.z) + L"\n";

		temp += L"\tTangent : "
			+ to_wstring(result[i].Tangent.x) +
			L", " + to_wstring(result[i].Tangent.y) +
			L", " + to_wstring(result[i].Tangent.z) + L"\n";

		OutputDebugString(temp.c_str()); // 비주얼 스튜디오 API임 그냥 여기에 찍어주는거
	}

	SAFE_RELEASE(dest);
}

void TestByteAddress::Destroy()
{
	SAFE_DELETE(shader);

	SAFE_RELEASE(input);
	SAFE_RELEASE(inputSRV);
	SAFE_RELEASE(output);
	SAFE_RELEASE(outputUAV);

	SAFE_RELEASE(result);
}

void TestByteAddress::Update()
{

}

void TestByteAddress::PreRender()
{
}

void TestByteAddress::Render()
{

}

void TestByteAddress::PostRender()
{
}

void TestByteAddress::ResizeScreen()
{
}
