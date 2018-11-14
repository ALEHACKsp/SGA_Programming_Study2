#include "stdafx.h"
#include "MeshGrid.h"

MeshGrid::MeshGrid(Material* material, float countX, float countZ, float sizeX, float sizeZ)
	: Mesh(material), countX(countX), countZ(countZ), sizeX(sizeX), sizeZ(sizeZ)
{

}

MeshGrid::~MeshGrid()
{

}

void MeshGrid::CreateData()
{
	float cX = countX + 1;
	float cZ = countZ + 1;

	float w = sizeX * 0.5f;
	float h = sizeZ * 0.5f;

	float x = sizeX / (cX - 1);
	float z = sizeZ / (cZ - 1);

	float u = 1.0f / (cX - 1);
	float v = 1.0f / (cZ - 1);

	vector<VertexTextureNormalTangent> vertices;
	for (float i = 0; i < cZ; i++)
	{
		float z1 = h - i * z;

		for (float k = 0; k < cX; k++)
		{
			float x1 = -w + k * x;

			VertexTextureNormalTangent vertex;
			vertex.Position = D3DXVECTOR3(x1, 0, z1);
			vertex.Uv = D3DXVECTOR2(k * u, i * v);
			vertex.Normal = D3DXVECTOR3(0, 1, 0);
			vertex.Tangent = D3DXVECTOR3(1, 0, 0);

			vertices.push_back(vertex);
		}
	}

	this->vertices = new VertexTextureNormalTangent[vertices.size()];
	vertexCount = vertices.size();
	copy(vertices.begin(), vertices.end(), stdext::checked_array_iterator<VertexTextureNormalTangent *>(this->vertices, vertexCount));


	vector<UINT> indices;
	for (float i = 0; i < cZ - 1; i++)
	{
		for (float k = 0; k < cX - 1; k++)
		{
			indices.push_back((UINT)(i * cX + k));
			indices.push_back((UINT)(i * cX + k + 1));
			indices.push_back((UINT)((i + 1) * cX + k));

			indices.push_back((UINT)((i + 1) * cX + k));
			indices.push_back((UINT)(i * cX + k + 1));
			indices.push_back((UINT)((i + 1) * cX + k + 1));
		}
	}

	this->indices = new UINT[indices.size()];
	indexCount = indices.size();
	copy(indices.begin(), indices.end(), stdext::checked_array_iterator<UINT *>(this->indices, indexCount));
}
