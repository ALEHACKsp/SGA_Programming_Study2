#include "stdafx.h"
#include "DebugLine.h"
#include "./Physics/Collider.h"

const UINT DebugLine::MaxCount = 1000;

DebugLine::DebugLine()
{
	shader = new Shader(Shaders + L"015_Line.hlsl");
	worldBuffer = new WorldBuffer();

	buffer = new Buffer();

	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(Vertex) * MaxCount * 2;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}
}

DebugLine::~DebugLine()
{
	SAFE_DELETE(shader);
	SAFE_DELETE(worldBuffer);

	SAFE_RELEASE(vertexBuffer);
}

void DebugLine::Draw(D3DXMATRIX& world, ILine * line)
{
	vector<D3DXVECTOR3> temp;
	line->GetLine(world, temp);

	lines.clear();

	for (size_t i = 0; i < temp.size(); i+=2)
	{
		Line line;
		line.start = temp[i + 0];
		line.end = temp[i + 1];
		lines.push_back(line);
	}

	D3D11_MAPPED_SUBRESOURCE subResource;
	HRESULT hr = D3D::GetDC()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource); // 메모리를 할당해놓고 다른것들이 접근할수 없도록 묶어줌.

	memcpy(subResource.pData, &lines[0], sizeof(Line) * lines.size()); // 벡터 시작부터 벡터 크기만큼 복사해줌

	D3D::GetDC()->Unmap(vertexBuffer, 0); // Unmap을 꼭 해주어야함. 
}


void DebugLine::Color(float r, float g, float b)
{
	Color(D3DXCOLOR(r, g, b, 1.0f));
}

void DebugLine::Color(D3DXCOLOR & vec)
{
	buffer->Data.Color = vec;
}

void DebugLine::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);


	buffer->SetPSBuffer(10);
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	D3D::GetDC()->Draw(lines.size() * 2, 0);

}
