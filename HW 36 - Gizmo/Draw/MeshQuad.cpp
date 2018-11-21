#include "stdafx.h"
#include "MeshQuad.h"

MeshQuad::MeshQuad(Material * material, D3DXVECTOR3 origin, D3DXVECTOR3 normal, 
	D3DXVECTOR3 up, float width, float height)
	: Mesh(material), origin(origin)
{
	D3DXVec3Normalize(&this->normal, &normal);
	D3DXVec3Normalize(&this->up, &up);

	D3DXVec3Cross(&left, &this->normal, &this->up);
	D3DXVECTOR3 uppercenter = (this->up * height / 2) + origin;
	upperLeft = uppercenter + (left * width / 2);
	upperRight = uppercenter - (left * width / 2);
	lowerLeft = upperLeft - (this->up * height);
	lowerRight = upperRight - (this->up * height);
}

MeshQuad::~MeshQuad()
{
}

void MeshQuad::CreateData()
{
	D3DXVECTOR2 uvUpperLeft = D3DXVECTOR2(0.0f, 0.0f);
	D3DXVECTOR2 uvUpperRight = D3DXVECTOR2(1.0f, 0.0f);
	D3DXVECTOR2 uvLowerLeft = D3DXVECTOR2(0.0f, 1.0f);
	D3DXVECTOR2 uvLowerRight = D3DXVECTOR2(1.0f, 1.0f);

	vector<VertexTextureNormalTangent> vertices;

	VertexTextureNormalTangent vertex;
	vertex.Normal = normal;
	vertex.Tangent = D3DXVECTOR3(1, 0, 0);

	vertex.Position = lowerLeft;
	vertex.Uv = uvLowerLeft;
	vertices.push_back(vertex);

	vertex.Position = upperLeft;
	vertex.Uv = uvUpperLeft;
	vertices.push_back(vertex);

	vertex.Position = lowerRight;
	vertex.Uv = uvLowerRight;
	vertices.push_back(vertex);

	vertex.Position = upperRight;
	vertex.Uv = uvUpperRight;
	vertices.push_back(vertex);

	this->vertices = new VertexTextureNormalTangent[vertices.size()];
	vertexCount = vertices.size();
	copy(vertices.begin(), vertices.end(), stdext::checked_array_iterator<VertexTextureNormalTangent *>(this->vertices, vertexCount));


	vector<UINT> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(3);
	
	this->indices = new UINT[indices.size()];
	indexCount = indices.size();
	copy(indices.begin(), indices.end(), stdext::checked_array_iterator<UINT *>(this->indices, indexCount));
}

void MeshQuad::Render()
{
	__super::Render();
}
