#pragma once

class ShaderBuffer
{
public:
	void SetVSBuffer(UINT slot);

	void SetPSBuffer(UINT slot);

protected:
	ShaderBuffer(void* data, UINT dataSize);

	virtual ~ShaderBuffer();

private:
	void MapData(void* data, UINT dataSize);

	D3D11_BUFFER_DESC desc;
	ID3D11Buffer* buffer;

	void* data;
	UINT dataSize;
};