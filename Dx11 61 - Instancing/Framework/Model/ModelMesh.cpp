#include "Framework.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"
#include "ModelBone.h"

ModelMesh::ModelMesh()
	: vertexBuffer(NULL)
{
	
}

ModelMesh::~ModelMesh()
{
	for (ModelMeshPart* part : meshParts)
		SAFE_DELETE(part);

	SAFE_DELETE_ARRAY(vertices);
	SAFE_RELEASE(vertexBuffer);
}

void ModelMesh::PreRender()
{
	for (ModelMeshPart* part : meshParts)
		part->PreRender();
}

void ModelMesh::Render()
{
	UINT stride = sizeof(VertexTextureNormalTangentBlend);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (ModelMeshPart* part : meshParts)
		part->Render();
}

void ModelMesh::Clone(void ** clone)
{
	ModelMesh * mesh = new ModelMesh();
	mesh->name = name;
	mesh->parentBoneIndex = parentBoneIndex;
	
	mesh->vertexCount = this->vertexCount;

	mesh->vertices = new VertexTextureNormalTangentBlend[this->vertexCount];
	memcpy(mesh->vertices, this->vertices, sizeof(VertexTextureNormalTangentBlend) * vertexCount);

	for (ModelMeshPart* part : meshParts)
	{
		ModelMeshPart* temp = NULL;
		part->Clone((void **)&temp);

		temp->parent = mesh;
		mesh->meshParts.push_back(temp);
	}

	*clone = mesh;
}

void ModelMesh::Pass(UINT val)
{
	for (ModelMeshPart* part : meshParts)
		part->Pass(val);
}

void ModelMesh::Binding()
{
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTextureNormalTangentBlend) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = vertices; // &vertices[0]랑 똑같은거임

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}
}