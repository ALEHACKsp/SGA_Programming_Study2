#pragma once

// executeValues 랑 viewport 미리컴파일헤더에 위에 붙을지 몰라서 앞에 붙인거

class RenderTarget
{
public:
	RenderTarget(struct ExecuteValues* values, UINT width = 0, UINT height = 0);
	~RenderTarget();

	void Set(D3DXCOLOR clear = 0xFF000000);

	ID3D11ShaderResourceView* GetSRV() { return srv; }

	// rtv로 렌더링 한 결과를 파일로 저장하기 위한 함수
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