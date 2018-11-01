#include "stdafx.h"
#include "BlendState.h"

BlendState::BlendState()
	: state(NULL)
{
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));

	// 섞을때 외곽선 부드럽게
	desc.AlphaToCoverageEnable = false; // 섞는거랑은 관련 없음
	// multi render target 그릴때는 켜줘야함 랜더타겟 독립적으로 쓸 수 있게 해줌
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = false;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	// 깊이 테스트 얘기할때 말씀 해주실꺼
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	Changed();
}

BlendState::~BlendState()
{
	SAFE_RELEASE(state);
}

void BlendState::AlphaToCoverageEnable(bool val)
{
	desc.AlphaToCoverageEnable = val;
	
	Changed();
}

void BlendState::BlendEnable(bool val)
{
	desc.RenderTarget[0].BlendEnable = val;

	Changed();
}

void BlendState::OMSetBlendState()
{
	D3D::GetDC()->OMSetBlendState(state, NULL, 0xFF);
}

void BlendState::BlendOp(D3D11_BLEND_OP val)
{
	desc.RenderTarget[0].BlendOp = val;

	Changed();
}

void BlendState::DestBlend(D3D11_BLEND val)
{
	desc.RenderTarget[0].DestBlend = val;

	Changed();
}

void BlendState::DestBlendAlpha(D3D11_BLEND val)
{
	desc.RenderTarget[0].DestBlendAlpha = val;

	Changed();
}

void BlendState::SrcBlend(D3D11_BLEND val)
{
	desc.RenderTarget[0].SrcBlend = val;

	Changed();
}

void BlendState::SrcBlendAlpha(D3D11_BLEND val)
{
	desc.RenderTarget[0].SrcBlendAlpha = val;

	Changed();
}

void BlendState::Changed()
{
	// 사실 이렇게 지우고 안해도 되긴함
	// 내부적으로 지워주긴 하는데
	SAFE_RELEASE(state);

	HRESULT hr = D3D::GetDevice()->CreateBlendState(&desc, &state);
	assert(SUCCEEDED(hr));
}
