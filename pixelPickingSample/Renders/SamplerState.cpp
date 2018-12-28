#include "stdafx.h"
#include "SamplerState.h"

SamplerState::SamplerState()
	: state(NULL)
{
	ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));

	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	//텍스쳐와 버텍스 차이로 인한 이미지 확대축소 보정 필터
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		//텍스쳐로 다 못채운 나머지 부분을 어떻게 채울 것인가
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MaxAnisotropy = 1;							//비등방성 필터링일 때 동작 - 성능좋은 보간
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	Changed();
}

SamplerState::~SamplerState()
{
	SAFE_RELEASE(state);
}

void SamplerState::PSSetSamplers(UINT slot)
{
	D3D::GetDC()->PSSetSamplers(slot, 1, &state);
}

void SamplerState::Filter(D3D11_FILTER val)
{
	desc.Filter = val;

	Changed();
}

void SamplerState::AddressU(D3D11_TEXTURE_ADDRESS_MODE val)
{
	desc.AddressU = val;

	Changed();
}

void SamplerState::AddressV(D3D11_TEXTURE_ADDRESS_MODE val)
{
	desc.AddressV = val;

	Changed();
}

void SamplerState::AddressW(D3D11_TEXTURE_ADDRESS_MODE val)
{
	desc.AddressW = val;

	Changed();
}

void SamplerState::ComparisonFunc(D3D11_COMPARISON_FUNC val)
{
	desc.ComparisonFunc = val;

	Changed();
}

void SamplerState::Changed()
{
	SAFE_RELEASE(state);

	HRESULT hr;

	hr = D3D::GetDevice()->CreateSamplerState(&desc, &state);
	assert(SUCCEEDED(hr));
}