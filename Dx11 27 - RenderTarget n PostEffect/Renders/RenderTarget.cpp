#include "stdafx.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(ExecuteValues * values, UINT width, UINT height)
	: values(values)
{
	// Render Target 크기 설정
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		this->width = (width < 1) ? (UINT)desc.Width : width;
		this->height = (height < 1) ? (UINT)desc.Height : height;
	}

	viewport = new Viewport((float)this->width, (float)this->height);

	D3D11_TEXTURE2D_DESC textureDesc;
	//CD3D11_TEXTURE2D_DESC 이걸로 하면 기본값 붙어잇는 거임

	// Create Texture - RTV용
	{
		ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		textureDesc.Width = this->width;
		textureDesc.Height = this->height;
		// mipmapping은 가까운건 선명하고 뒤에껀 흐려지게 할 때 쓰기 위해 여러장쓰는거
		textureDesc.MipLevels = 1; // mipmapping 안쓸꺼
		textureDesc.ArraySize = 1; // texture안에 여러 장 들어갈 수 있음
		// 정밀도 높여놓은 담에 렌더링하고 줄일려고 이렇게하는거
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 총 128비트 사용하는거
		textureDesc.SampleDesc.Count = 1; // sampling 확대 축소 할 때 간격 맞춰주고 하는거
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage - D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE
			| D3D11_BIND_RENDER_TARGET; // 이게 shader resource view로 할당하겠다는거 
		// render target 용도로도 쓰겠다는거

		// 1d 는 단색으로 1차원 배열로 들어가는거
		// 3d 는 3차원
		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&textureDesc, NULL, &rtvTexture);
		assert(SUCCEEDED(hr));
	}

	// Create RTV
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		desc.Format = textureDesc.Format;
		// texture 차원 결정 3d (d가 이 dimension)
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0; // mipmapping에 따라 2d 잘라놓을꺼냐는거 mipmapping 안쓰니까 0

		// texture 리소스로 취급
		HRESULT hr = D3D::GetDevice()->CreateRenderTargetView(rtvTexture, &desc, &rtv);
		assert(SUCCEEDED(hr));
	}

	// Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = textureDesc.Format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;
		
		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(rtvTexture, &desc, &srv);
		assert(SUCCEEDED(hr));
	}

	// Create Texture - DSV용
	{
		textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&textureDesc, NULL, &dsvTexture);
		assert(SUCCEEDED(hr));
	}

	// Create DSV
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		desc.Format = textureDesc.Format;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		HRESULT hr = D3D::GetDevice()->CreateDepthStencilView(dsvTexture, &desc, &dsv);
		assert(SUCCEEDED(hr));
	}
}

RenderTarget::~RenderTarget()
{
	SAFE_RELEASE(rtvTexture);
	SAFE_RELEASE(dsvTexture);

	SAFE_RELEASE(srv);
	SAFE_RELEASE(rtv);
	SAFE_RELEASE(dsv);

	SAFE_DELETE(viewport);
}

void RenderTarget::Set(D3DXCOLOR clear)
{
	D3D::Get()->SetRenderTarget(rtv, dsv);
	D3D::Get()->Clear(clear, rtv, dsv);

	viewport->RSSetViewport();
}

void RenderTarget::SaveRtvTexture(wstring saveFile)
{
	HRESULT hr = D3DX11SaveTextureToFile(
		D3D::GetDC(), rtvTexture, D3DX11_IFF_PNG, saveFile.c_str());
	assert(SUCCEEDED(hr));
}
