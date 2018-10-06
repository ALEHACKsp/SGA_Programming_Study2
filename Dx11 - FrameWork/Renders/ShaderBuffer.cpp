#include "stdafx.h"
#include "ShaderBuffer.h"

void ShaderBuffer::SetVSBuffer(UINT slot)
{
	MapData(data, dataSize);

	D3D::GetDC()->VSSetConstantBuffers(slot, 1, &buffer);
}

void ShaderBuffer::SetPSBuffer(UINT slot)
{
	MapData(data, dataSize);

	D3D::GetDC()->PSSetConstantBuffers(slot, 1, &buffer);
}

ShaderBuffer::ShaderBuffer(void * data, UINT dataSize)
	: data(data), dataSize(dataSize)
{
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = dataSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &buffer);
	assert(SUCCEEDED(hr));
}

ShaderBuffer::~ShaderBuffer()
{
	SAFE_RELEASE(buffer);
}

void ShaderBuffer::MapData(void * data, UINT dataSize)
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	HRESULT hr = D3D::GetDC()->Map
	(
		buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
	);

	memcpy(subResource.pData, data, dataSize);

	D3D::GetDC()->Unmap(buffer, 0);
}
