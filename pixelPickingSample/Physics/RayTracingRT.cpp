#include "stdafx.h"
#include "RayTracingRT.h"

RayTracingRT::RayTracingRT(ExecuteValues * values)
	: values(values)
{
	renderTarget = new RenderTarget(values, (UINT)values->Viewport->GetWidth(), (UINT)values->Viewport->GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM);
	shader = new Shader(Shaders + L"047_TerrainRayTracing.hlsl");
	render2D = new Render2D(values);
	render2D->Position(0, 100);
	render2D->Scale(200, 100);

	buffer = new Buffer();
	buffer->Data.Width = 256;
	buffer->Data.Height = 256;
}

RayTracingRT::~RayTracingRT()
{
}

void RayTracingRT::PreRender(ID3D11Buffer * vertexBuffer, ID3D11Buffer * indexBuffer, UINT indexCount)
{
	renderTarget->Set();
	ID3D11RenderTargetView* rtv;
	rtv = renderTarget->GetRTV();
	ID3D11DepthStencilView* dsv;
	dsv = renderTarget->GetDSV();
	D3D::Get()->SetRenderTarget(rtv, dsv);

	UINT stride = sizeof(VertexTextureNormalTangentSplatting);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	buffer->SetVSBuffer(10);
	shader->Render();
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

bool RayTracingRT::Picking(OUT D3DXVECTOR3* val)
{
	UINT x, y;
	D3DXVECTOR3 position = Mouse::Get()->GetPosition();
	UINT result = Texture::ReadPixel32Bit(renderTarget->GetTexture2D(), (UINT)position.x, (UINT)position.y);
	x = (0x000000FF & result) >> 0;
	y = (0x0000FF00 & result) >> 8;
	(*val) = D3DXVECTOR3((float)x, 0, (float)y);
	return x + y > 0;
}
