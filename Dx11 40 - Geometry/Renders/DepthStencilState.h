#pragma once

class DepthStencilState
{
public:
	DepthStencilState();
	~DepthStencilState();

	void OMSetDepthStencilState(UINT stencilRef = 1);

	void DepthEnable(bool val);
	void DepthWriteEnable(D3D11_DEPTH_WRITE_MASK val); // 깊이를 쓰는 용도 테스트 시켜서
	void StencilEnable(bool val);

private:
	void Changed();

private:
	D3D11_DEPTH_STENCIL_DESC desc;

	ID3D11DepthStencilState* state;
};