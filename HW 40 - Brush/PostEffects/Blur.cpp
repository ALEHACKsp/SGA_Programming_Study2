#include "stdafx.h"
#include "Blur.h"

Blur::Blur(wstring shaderFile, UINT width, UINT height, DXGI_FORMAT format, int blurCount)
	:width(width), height(height), blurCount(blurCount)
{
	shader = new Shader(shaderFile, true);
	shader->AsScalar("BlurRadius")->SetInt(blurCount);

	HRESULT hr;

	// texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texture;
	hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture);
	assert(SUCCEEDED(hr));

	// srv
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = D3D::GetDevice()->CreateShaderResourceView(texture, &srvDesc, &srv);
	assert(SUCCEEDED(hr));

	// uav
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	hr = D3D::GetDevice()->CreateUnorderedAccessView(texture, &uavDesc, &uav);
	assert(SUCCEEDED(hr));

	SAFE_RELEASE(texture);
}

Blur::~Blur()
{
	SAFE_DELETE(shader);
	SAFE_RELEASE(srv);
	SAFE_RELEASE(uav);
}

void Blur::SetWeights(float sigma)
{
	float d = 2.0f * sigma * sigma;

	float* weights = new float[blurCount];
	float sum = 0.0f;

	for (int i = 0; i < blurCount; i++) 
	{
		weights[i] = expf((float)-i * (float)i / d);

		sum += weights[i];
	}

	for (int i = 0; i < blurCount; i++)
		weights[i] /= sum;

	shader->AsScalar("Weights")->SetFloatArray(weights, 0, blurCount);

	SAFE_DELETE(weights);
}

void Blur::Dispatch(ID3D11ShaderResourceView * inputSRV, ID3D11UnorderedAccessView * inputUAV)
{
	UINT group = 0;

	for (int i = 0; i < blurCount; i++) 
	{
		shader->AsShaderResource("Input")->SetResource(inputSRV); // 원본 rtv
		shader->AsUAV("Output")->SetUnorderedAccessView(uav);

		group = (UINT)ceilf(width / 256.0f);
		shader->Dispatch(0, 0, group, height, 1);

		shader->AsShaderResource("Input")->SetResource(srv); // 원본 rtv
		shader->AsUAV("Output")->SetUnorderedAccessView(inputUAV);

		group = (UINT)ceilf(height / 256.0f);
		shader->Dispatch(0, 0, width, group, 1);
	}
}
