#include "Framework.h"
#include "CsResource.h"


void CsResource::CreateRawBuffer(UINT size, void * initData, ID3D11Buffer ** buffer)
{
	D3D11_BUFFER_DESC desc = { 0 };
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D10_BIND_VERTEX_BUFFER;
	desc.ByteWidth = size;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	HRESULT hr;
	if (initData != NULL)
	{
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = initData;

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, buffer);
	}
	else
		hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, buffer);
	assert(SUCCEEDED(hr));

}

void CsResource::CreateStructuredBuffer(UINT size, UINT count, void * initData, ID3D11Buffer ** buffer)
{
	D3D11_BUFFER_DESC desc = { 0 };
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = size * count;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = size;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	HRESULT hr;
	if (initData != NULL)
	{
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = initData;

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, buffer);
	}
	else
		hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, buffer);
	assert(SUCCEEDED(hr));
}

void CsResource::CreateSRV(ID3D11Buffer * buffer, ID3D11ShaderResourceView ** srv)
{
	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   //   BufferEx ���߸� structured buffer�� ��
	srvDesc.BufferEx.FirstElement = 0;

	//    & : == �̶� ����
	if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS/*������� ��� ����*/)
	{
		srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		srvDesc.BufferEx.NumElements = desc.ByteWidth / 4;
	}
	else if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.BufferEx.NumElements = desc.ByteWidth / desc.StructureByteStride;
	}

	HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(buffer, &srvDesc, srv);
	assert(SUCCEEDED(hr));
}

void CsResource::CreateSRV(ID3D11Texture2D * texture, ID3D11ShaderResourceView ** srv)
{
	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	//srvDesc.Buffer.FirstElement = 0;

	////    & : == �̶� ����
	//if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS/*������ ��� ����*/)
	//{
	//	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_TYPELESS;
	//	srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	//	srvDesc.BufferEx.NumElements = desc.Width * desc.Height;
	//}

	HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(texture, &srvDesc, srv);
	assert(SUCCEEDED(hr));
}

void CsResource::CreateUAV(ID3D11Buffer * buffer, ID3D11UnorderedAccessView ** uav)
{
	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;

	//    & : == �̶� ����
	if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS/*������ ��� ����*/)
	{
		uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		uavDesc.Buffer.NumElements = desc.ByteWidth / 4;
	}
	else if (desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
	{
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;
	}

	HRESULT hr = D3D::GetDevice()->CreateUnorderedAccessView(buffer, &uavDesc, uav);
	assert(SUCCEEDED(hr));
}

ID3D11Buffer * CsResource::CreateAndCopyBuffer(ID3D11Buffer * src)
{
	ID3D11Buffer* dest;

	D3D11_BUFFER_DESC desc = { 0 };
	src->GetDesc(&desc);

	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.MiscFlags = 0;
	desc.BindFlags = 0;
	
	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &dest);
	assert(SUCCEEDED(hr));

	D3D::GetDC()->CopyResource(dest, src);

	return dest;
}