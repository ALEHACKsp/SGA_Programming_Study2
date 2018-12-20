#include "stdafx.h"
#include "QuadTree.h"
#include "Terrain.h"
#include "../Viewer/Frustum.h"

QuadTree::QuadTree(Terrain * terrain, Frustum* frustum, Shader* shader)
	: terrain(terrain), frustum(frustum), nodes{ NULL }, shader(shader)
	, indexBuffer(NULL), bottom(FLT_MAX), top(FLT_MIN), indices(NULL)
{
}

QuadTree::~QuadTree()
{
	for (UINT i = 0; i < 4; i++)
		SAFE_DELETE(nodes[i]);

	SAFE_DELETE_ARRAY(indices);
	SAFE_RELEASE(indexBuffer);
}

void QuadTree::Render()
{
	if (frustum->ContainCube(centerPosition, width * 0.5f) == false)
	   return;

	for (UINT i = 0; i < 4; i++)
	{
		if (nodes[i] != NULL)
			nodes[i]->Render();
	}

	if (width > terrain->patchSize)
		return;

	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	shader->DrawIndexed(0, 0, indexCount);
}

void QuadTree::Build(D3DXVECTOR2 & center, float w)
{
	this->width = w + 1;

	centerPosition.x = center.x;
	centerPosition.z = center.y;

	float half = (int)this->width / 2;
	if (width > terrain->patchSize)
	{
		for (int i = 0; i < 4; i++)
		{
			D3DXVECTOR2 nextCenter;
			nextCenter.x = center.x + ((i % 2 < 1) ? -1 : 1) * (half / 2);
			nextCenter.y = center.y + ((i < 2) ? -1 : 1) * (half / 2);

			nodes[i] = new QuadTree(terrain, frustum, shader);
			nodes[i]->Build(nextCenter, half);


			if (nodes[i]->bottom < bottom)
				bottom = nodes[i]->bottom;

			if (nodes[i]->top > top)
				top = nodes[i]->top;
		}
		centerPosition.y = bottom + ((top - bottom) / 2);

		return;
	}

	int index = 0;
	int height = terrain->height;
	int endZ = center.y + half;
	int endX = center.x + half;

	indexCount = width * width * 4;
	indices = new UINT[indexCount];
	for (int z = center.y - half; z < endZ; z++)
	{
		for (int x = center.x - half; x < endX; x++)
		{
			indices[index + 0] = (height + 1) * z + x;
			indices[index + 1] = (height + 1) * (z + 1) + x;

			indices[index + 2] = (height + 1) * z + x + 1;
			indices[index + 3] = (height + 1) * (z + 1) + x + 1;

			for (int i = 0; i < 4; i++)
			{
				float temp = terrain->vertices[indices[index + i]].Position.y;

				if (temp < bottom) bottom = temp;
				if (temp > top) top = temp;
			}

			index += 4;
		}
	}
	centerPosition.y = bottom + ((top - bottom) / 2);

	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = indices;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));
}