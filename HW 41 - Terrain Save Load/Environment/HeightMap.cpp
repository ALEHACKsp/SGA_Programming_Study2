#include "stdafx.h"
#include "HeightMap.h"

#include "Utilities/BinaryFile.h"

HeightMap::HeightMap(UINT width, UINT height, float maxHeight)
	: width(width), height(height), maxHeight(maxHeight)
{
	datas = new float[width * height];

	saveShader = new Shader(Shaders + L"Homework/SaveHeightMap.fx", true);
	
	cOutput = new CsResource(sizeof(ComputeBuffer), width * height, NULL);
}

HeightMap::~HeightMap()
{
	SAFE_DELETE_ARRAY(datas);

	SAFE_DELETE(saveShader);
	SAFE_DELETE(cOutput);
}

void HeightMap::Load(wstring file)
{
	SAFE_DELETE_ARRAY(datas);
	datas = new float[width * height];

	BinaryReader* r = new BinaryReader();
	r->Open(file);

	BYTE* temp = new BYTE[width * height];
	r->Byte((void**)&temp, width * height);

	r->Close();
	SAFE_DELETE(r);

	for (int i = 0; i < (int)(width * height); i++)
		datas[i] = (float)temp[i] / 255.0f * maxHeight;
		//datas[i] = 0;
		//datas[i] = 10;

	SAFE_DELETE_ARRAY(temp);
}

void HeightMap::Save(wstring file, ID3D11Texture2D* tex)
{
	D3D11_TEXTURE2D_DESC srcDesc;
	tex->GetDesc(&srcDesc);

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = srcDesc.Width;
	desc.Height = srcDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_FLOAT;
	desc.SampleDesc = srcDesc.SampleDesc;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;

	HRESULT hr;

	ID3D11Texture2D* texture;
	hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture);
	assert(SUCCEEDED(hr));

	hr = D3DX11LoadTextureFromTexture(D3D::GetDC(), tex, NULL, texture);
	assert(SUCCEEDED(hr));

	float* temp = new float[width * height];

	D3D11_MAPPED_SUBRESOURCE map;
	D3D::GetDC()->Map(texture, 0, D3D11_MAP_READ, NULL, &map);
	{
		memcpy(temp, map.pData, sizeof(float) * width * height);
	}
	D3D::GetDC()->Unmap(texture, 0);

	SAFE_DELETE_ARRAY(datas);
	datas = new float[width * height];

	for (int i = 0; i < (int)(width * height); i++)
		datas[i] = (float)temp[i];

	//D3DXFloat16To32Array(datas, temp, width * height);

	//for (int i = 0; i < (int)(width * height); i++)
	//	datas[i] = (float)datas[i] * 255.0f / maxHeight;

	//BinaryWriter* w = new BinaryWriter();

	//w->Open(file);
	//
	//w->Byte(datas, width * height);

	//w->Close();

	//SAFE_DELETE(w);

	SAFE_DELETE_ARRAY(temp);

	SAFE_RELEASE(texture);
}

void HeightMap::Save(wstring file, ID3D11ShaderResourceView * srv)
{
	saveShader->AsShaderResource("Input")->SetResource(srv);
	saveShader->AsUAV("Output")->SetUnorderedAccessView(cOutput->UAV());

	saveShader->Dispatch(0, 0, 256, 256, 1);

	BYTE* temp = new BYTE[width * height];

	vector<ComputeBuffer> buffer(width * height);
	cOutput->Read(&buffer[0]);

	for (int i = 0; i < width * height; i++)
		temp[i] = (BYTE)(buffer[i].data * 255.0f / maxHeight);

	//D3DXFLOAT16* temp = new D3DXFLOAT16[width * height];

	//for (int i = 0; i < width * height; i++)
	//	temp[i] = buffer[i].data;

	//D3DXFloat16To32Array(datas, temp, width * height);

	//SAFE_DELETE_ARRAY(temp);

	BinaryWriter* w = new BinaryWriter();

	w->Open(file);
	
	w->Byte(temp, width * height);

	w->Close();

	SAFE_DELETE(w);

	SAFE_DELETE_ARRAY(temp);
}

void HeightMap::Data(UINT row, UINT col, float data)
{
	if (InBounds(row, col) == false) 
		return;

	datas[row * height + col] = data;
}

float HeightMap::Data(UINT row, UINT col)
{
	if (InBounds(row, col) == false)
		return 0.0f;

	return datas[row * height + col];
}

bool HeightMap::InBounds(int row, int col)
{
	return row >= 0 && row < (int)height && col >= 0 && col < (int)width;
}

void HeightMap::Smooth()
{
	float* dest = new float[width * height];
	for (UINT row = 0; row < height; row++)
	{
		for (UINT col = 0; col < width; col++)
			dest[row * height + col] = Average(row, col);
	}

	memcpy(datas, dest, sizeof(float) * width * height);
	SAFE_DELETE_ARRAY(dest);
}

float HeightMap::Average(UINT row, UINT col)
{
	float average = 0.0f;
	float sum = 0.0f;

	for (UINT m = row - 1; m <= row + 1; m++)
	{
		for (UINT n = col - 1; n <= col + 1; n++)
		{
			if (InBounds(m, n) == false) continue;

			average += datas[m * height + n];
			sum++;
		}
	}

	return average / sum;
}

void HeightMap::Build(OUT ID3D11Texture2D** tex, OUT ID3D11ShaderResourceView** srv, OUT ID3D11UnorderedAccessView** uav)
{
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.Format = DXGI_FORMAT_R32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	//D3DXFLOAT16* temp = new D3DXFLOAT16[width * height];

	//D3DXFloat32To16Array(temp, datas, width*height);

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = datas;
	data.SysMemPitch = width * sizeof(float);

	HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, &data, tex);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc; // 안에 포인터 있으면 0 초기화 불가능, 생성자가 없어도 안됨
	// union 그 구조체 안에 가장 큰 사이즈로 맞추고 나머지는 그 공간 공유해서 쓰는거
	// union에서도 padding 잡아줄 수 있음
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	hr = D3D::GetDevice()->CreateShaderResourceView(*tex, &srvDesc, srv);
	assert(SUCCEEDED(hr));

	//SAFE_DELETE_ARRAY(temp);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = desc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	
	hr = D3D::GetDevice()->CreateUnorderedAccessView(*tex, &uavDesc, uav);
	assert(SUCCEEDED(hr));
}
