#include "Framework.h"
#include "CubeSky.h"
#include "../Viewer/Camera.h"

CubeSky::CubeSky(wstring cubeFile)
{
	material = new Material(Shaders + L"056_CubeSky.fx");
	sphere = new MeshSphere(material, 500.0f, 30, 30);

	wstring file = cubeFile;
	
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
		D3D::GetDevice(), file.c_str(), NULL, NULL, &srv, NULL);
	assert(SUCCEEDED(hr));

	//ID3D11Texture2D* texture;
	//srv->GetResource((ID3D11Resource **)&texture);

	//D3D11_TEXTURE2D_DESC textureDesc;
	//texture->GetDesc(&textureDesc);

	//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	//srv->GetDesc(&srvDesc);

	material->GetShader()->AsShaderResource("CubeMap")->SetResource(srv);
}

CubeSky::~CubeSky()
{
	SAFE_DELETE(sphere);
	SAFE_DELETE(material);

	SAFE_RELEASE(srv);
}

void CubeSky::Update()
{
	D3DXVECTOR3 position;
	Context::Get()->GetMainCamera()->Position(&position);

	sphere->Position(position);
}

void CubeSky::PreRender()
{
	sphere->PreRender();
}

void CubeSky::Render()
{
	sphere->Render();
}