#pragma once

class RenderTarget
{
public:
	RenderTarget(struct ExecuteValues* values, UINT width = 0, UINT height = 0, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);
	~RenderTarget();

	void Set(D3DXCOLOR clear = 0xFF000000);

	ID3D11ShaderResourceView* GetSRV() { return srv; }
	ID3D11RenderTargetView* GetRTV() { return rtv; }
	ID3D11DepthStencilView* GetDSV() { return dsv; }
	ID3D11Texture2D* GetTexture2D() { return rtvTexture; }

	void SaveRtvTexture(wstring saveFile);
	

private:
	struct ExecuteValues* values;

	UINT width, height;

	ID3D11ShaderResourceView* srv;
	ID3D11Texture2D* rtvTexture;
	ID3D11RenderTargetView* rtv;

	ID3D11Texture2D* dsvTexture;
	ID3D11DepthStencilView* dsv;

	class Viewport* viewport;
};
