#include "Framework.h"
#include "GameModel.h"

#include "../Model/ModelMesh.h"
#include "../Model/ModelMeshPart.h"

GameModel::GameModel(wstring shaderFile, wstring matFile, wstring meshFile)
	:shaderFile(shaderFile), matFile(matFile), meshFile(meshFile)
{
	shader = new Shader(Shaders + L"064_CsModel.fx", true);

	pass = 0;

	//model->CopyGlobalBoneTo(transforms);
}

GameModel::~GameModel()
{
	SAFE_DELETE(model);

	SAFE_DELETE(shader);

	SAFE_RELEASE(boneBuffer);
	SAFE_RELEASE(boneSrv);

	SAFE_DELETE_ARRAY(boneTransforms);
	SAFE_DELETE_ARRAY(renderTransforms);
}

void GameModel::Ready()
{
	model = new Model();
	model->ReadMaterial(matFile);
	model->ReadMesh(meshFile);

	SetShader(shaderFile);

	boneTransforms = new D3DXMATRIX[model->BoneCount()];
	renderTransforms = new D3DXMATRIX[model->BoneCount()];

	CsResource::CreateStructuredBuffer(sizeof(D3DXMATRIX), model->BoneCount(), NULL, &boneBuffer);
	CsResource::CreateSRV(boneBuffer, &boneSrv);

	boneIndexVariable = shader->AsScalar("BoneIndex");
	vertexVariable = shader->AsSRV("Vertex");
	resultVariable = shader->AsUAV("Result");

	boneBufferVariable = shader->AsSRV("BoneBuffer");

	UpdateWorld();
}

void GameModel::Update()
{

}

void GameModel::PreRender()
{
	//for (Material* material : model->Materials())
	//{
	//	const float* data = transforms[0];
	//	UINT count = transforms.size();

	//	material->GetShader()->AsMatrix("Bones")->SetMatrixArray(data, 0, count);
	//}

	//for (ModelMesh* mesh : model->Meshes())
	//	mesh->PreRender();
}

void GameModel::Render()
{
	//for (Material* material : model->Materials())
	//{
	//	const float* data = transforms[0];
	//	UINT count = transforms.size();

	//	material->GetShader()->AsMatrix("Bones")->SetMatrixArray(data, 0, count);
	//}

	for (ModelMesh* mesh : model->Meshes())
		mesh->Render();
}

#pragma region Material
void GameModel::SetShader(wstring shaderFiler)
{
	for (Material* material : model->Materials())
		material->SetShader(shaderFiler);
}

void GameModel::SetDiffuse(float r, float g, float b, float a)
{
	for (Material* material : model->Materials())
		material->SetDiffuse(D3DXCOLOR(r, g, b, a));
}

void GameModel::SetDiffuse(D3DXCOLOR & color)
{
	for (Material* material : model->Materials())
		material->SetDiffuse(color);
}

void GameModel::SetDiffuseMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetDiffuseMap(file);
}

void GameModel::SetSpecular(float r, float g, float b, float a)
{
	for (Material* material : model->Materials())
		material->SetSpecular(D3DXCOLOR(r, g, b, a));
}

void GameModel::SetSpecular(D3DXCOLOR & color)
{
	for (Material* material : model->Materials())
		material->SetSpecular(color);
}

void GameModel::SetSpecularMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetSpecularMap(file);
}

void GameModel::SetNormalMap(wstring file)
{
	for (Material* material : model->Materials())
		material->SetNormalMap(file);
}

void GameModel::SetShininess(float val)
{
	for (Material* material : model->Materials())
		material->SetShininess(val);
}
#pragma endregion

void GameModel::UpdateWorld()
{
	__super::UpdateWorld();

	for (Material* material : model->Materials())
		material->SetWorld(World());

	//D3DXMATRIX W = World();
	//model->CopyGlobalBoneTo(transforms, W);

	UpdateTransforms();
	MappedBoneBuffer();
	UpdateVertex();
}

void GameModel::UpdateVertex()
{
	for (ModelMesh* mesh : model->Meshes())
	{
		for (ModelMeshPart* part : mesh->MeshParts())
		{
			vector<VertexTextureNormalTangentBlend> vertices;
			part->Vertices(vertices);

			// Mesh Part에 있어줘야함 테스트라 일단 여기서 하는거
			ID3D11Buffer* vertexBuffer;
			ID3D11ShaderResourceView* vertexSrv;
			ID3D11UnorderedAccessView* vertexUav;

			CsResource::CreateRawBuffer(
				sizeof(VertexTextureNormalTangentBlend) * vertices.size(),
				&vertices[0], &vertexBuffer);

			CsResource::CreateSRV(vertexBuffer, &vertexSrv);
			CsResource::CreateUAV(part->VertexBuffer(), &vertexUav);

			boneIndexVariable->SetInt(mesh->ParentBoneIndex());
			vertexVariable->SetResource(vertexSrv);
			resultVariable->SetUnorderedAccessView(vertexUav);

			float x = ceilf((float)(vertices.size()) / 512.0f);
			shader->Dispatch(0, pass, (UINT)x, 1, 1);

			SAFE_RELEASE(vertexSrv);
			SAFE_RELEASE(vertexUav);
			SAFE_RELEASE(vertexBuffer);
		}
	}
}

void GameModel::UpdateTransforms()
{
	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->BoneByIndex(i);

		D3DXMATRIX parentTransform;

		int parentIndex = bone->ParentIndex();
		if (parentIndex < 0)
			D3DXMatrixIdentity(&parentTransform);
		else
			parentTransform = boneTransforms[parentIndex];

		boneTransforms[i] = parentTransform;
		renderTransforms[i] = bone->Global() * boneTransforms[i];
	}
}

void GameModel::MappedBoneBuffer()
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(boneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, renderTransforms, sizeof(D3DXMATRIX) * model->BoneCount());
	}
	D3D::GetDC()->Unmap(boneBuffer, 0);

	boneBufferVariable->SetResource(boneSrv);
}
