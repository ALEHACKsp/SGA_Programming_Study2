#include "stdafx.h"
#include "QuadTree.h"
#include "Terrain.h"

const UINT QuadTree::DivideCount = 32 * 32; // 1024

QuadTree::QuadTree(Terrain* terrain)
{
	UINT vertexCount = terrain->VertexCount();
	triangleCount = vertexCount / 3;

	vertices = new VertexTextureNormal[vertexCount];
	terrain->CoptyVertices((void *)vertices);

	float centerX = 0.0f, centerZ = 0.0f, width = 0.0f;
	CalcMeshDimensions(vertexCount, centerX, centerZ, width);

	parent = new NodeType();

}

QuadTree::~QuadTree()
{
}

void QuadTree::Update()
{
}

void QuadTree::Render()
{
}

void QuadTree::CalcMeshDimensions(UINT vertexCount, float & centerX, float & centerZ, float & meshWidth)
{
	centerX = centerZ = 0.0f;

	for (UINT i = 0; i < vertexCount; i++)
	{
		centerX += vertices[i].Position.x;
		centerZ += vertices[i].Position.z;
	}

	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;

	float maxWidth = 0.0f;
	float maxDepth = 0.0f;

	float minWidth = fabsf(vertices[0].Position.x - centerX);
	float minDepth = fabsf(vertices[0].Position.z - centerZ);

	for (UINT i = 0; i < vertexCount; i++)
	{
		float width = fabsf(vertices[i].Position.x - centerX);
		float depth = fabsf(vertices[i].Position.z - centerZ);

		if (width > maxWidth) maxWidth = width;
		if (depth > maxDepth) maxDepth = depth;
		if (width < minWidth) minWidth = width;
		if (depth < minDepth) minDepth = depth;
	}

	float maxX = (float)max(fabsf(minWidth), fabsf(maxWidth));
	float maxZ = (float)max(fabsf(minDepth), fabsf(maxDepth));

	// mesh 최대 직경 계산
	meshWidth = max(maxX, maxZ) * 2.0f;
}

void QuadTree::CreateTreeNode(NodeType * node, float positionX, float positionZ, float width)
{
	node->x = positionX;
	node->z = positionZ;
	node->width = width;

	node->triangleCount = 0;

	node->vertexBuffer = NULL;
	node->indexBuffer = NULL;

	for (UINT i = 0; i < 4; i++)
		node->childs[i] = NULL;

	UINT triangles = ContainTriangleCount(positionX, positionZ, width);

	// Case 1 : 남은 갯수가 없을 때
	if (triangles == 0)
		return;

	// Case 2 : 더 작은 노드로 분할
	if (triangles > DivideCount)
	{
		for (UINT i = 0; i < 4; i++)
		{
			float offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
			float offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

			UINT count = 
				ContainTriangleCount((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			if (count > 0)
			{
				node->childs[i] = new NodeType();

				CreateTreeNode(node->childs[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			}
		}

		return;
	}

	// Case 3 : 남은 갯수가 있을 때
	node->triangleCount = triangles;

	UINT vertexCount = triangles * 3;
	VertexTextureNormal* vertices = new VertexTextureNormal[vertexCount];
	UINT* indices = new UINT[vertexCount];

	UINT index = 0, vertexIndex = 0;
	for (UINT i = 0; i < triangleCount; i++)
	{
		if (IsTriangleContained(i, positionX, positionZ, width) == true)
		{
			vertexIndex = i * 3;

			vertices[index].Position = this->vertices[vertexIndex].Position;
			vertices[index].Uv = this->vertices[vertexIndex].Uv;
			vertices[index].Normal = this->vertices[vertexIndex].Normal;
			indices[index] = index;
			index++;

			vertexIndex++;
			vertices[index].Position = this->vertices[vertexIndex].Position;
			vertices[index].Uv = this->vertices[vertexIndex].Uv;
			vertices[index].Normal = this->vertices[vertexIndex].Normal;
			indices[index] = index;
			index++;

			vertexIndex++;
			vertices[index].Position = this->vertices[vertexIndex].Position;
			vertices[index].Uv = this->vertices[vertexIndex].Uv;
			vertices[index].Normal = this->vertices[vertexIndex].Normal;
			indices[index] = index;
			index++;
		}
	}
}

UINT QuadTree::ContainTriangleCount(float positionX, float positionZ, float width)
{
	UINT count = 0;

	for (UINT i = 0; i < triangleCount; i++)
	{
		if (IsTriangleContained(i, positionX, positionZ, width) == true)
			count++;
	}

	return count;
}

bool QuadTree::IsTriangleContained(UINT index, float positionX, float positionZ, float width)
{
	float radius = width / 2.0f;
	UINT vertexIndex = index * 3;
	float x1 = vertices[vertexIndex].Position.x;
	float z1 = vertices[vertexIndex].Position.z;
	vertexIndex++;

	float x2 = vertices[vertexIndex].Position.x;
	float z2 = vertices[vertexIndex].Position.z;
	vertexIndex++;

	float x3 = vertices[vertexIndex].Position.x;
	float z3 = vertices[vertexIndex].Position.z;

	float minimumX = min(x1, min(x2, x3));
	if (minimumX > (positionX + radius))
		return false;

	float maximumX = max(x1, max(x2, x3));
	if (maximumX < (positionX - radius))
		return false;

	float minimumZ = min(z1, min(z2, z3));
	if (minimumZ >(positionZ + radius))
		return false;

	float maximumZ = max(z1, max(z2, z3));
	if (maximumZ < (positionZ - radius))
		return false;

	return true;
}
