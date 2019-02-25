#pragma once

class CsResource
{
public:
	static void CreateRawBuffer(UINT size, void *initData, ID3D11Buffer ** buffer);
	static void CreateRawBuffer2(UINT size, void *initData, ID3D11Buffer ** buffer);
	static void CreateStructuredBuffer(UINT size, UINT count, void* initData, ID3D11Buffer ** buffer);
	static void CreateStructuredBuffer2(UINT size, UINT count, void* initData, ID3D11Buffer ** buffer);
	static void CreateSRV(ID3D11Buffer* buffer, ID3D11ShaderResourceView** srv);
	static void CreateSRV(ID3D11Texture2D* texture, ID3D11ShaderResourceView** srv);
	static void CreateUAV(ID3D11Buffer* buffer, ID3D11UnorderedAccessView** uav);
	static ID3D11Buffer* CreateAndCopyBuffer(ID3D11Buffer* src);
	static void Read(ID3D11Buffer* buffer, UINT byteWidth, OUT void * data);
};