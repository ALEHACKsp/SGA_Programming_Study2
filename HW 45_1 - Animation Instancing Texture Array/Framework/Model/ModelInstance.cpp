#include "FrameWork.h"
#include "ModelInstance.h"

#include "Model.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"
#include "ModelBone.h"

ModelInstance::ModelInstance(Model * model, wstring shaderFile)
	: maxCount(0), model(model)
{
	for (Material* material : model->Materials())
		material->SetShader(shaderFile);
}

ModelInstance::~ModelInstance()
{
	SAFE_RELEASE(transTexture);
	SAFE_RELEASE(transSrv);
}

void ModelInstance::AddWorld(D3DXMATRIX& world)
{
	worlds[maxCount] = world;

	maxCount++;
	
	D3D11_MAPPED_SUBRESOURCE subResource;

	for (UINT i = 0; i < model->MeshCount(); i++)
	{
		ID3D11Buffer* instanceBuffer = model->MeshByIndex(i)->InstanceBuffer();

		D3D::GetDC()->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			memcpy(subResource.pData, worlds, sizeof(D3DXMATRIX) * maxCount);
		}
		D3D::GetDC()->Unmap(instanceBuffer, 0);
	}

	for (Material* material: model->Materials())
		material->GetShader()->AsSRV("Transforms")->SetResource(transSrv);
}

void ModelInstance::Ready()
{
	{
		D3D11_TEXTURE2D_DESC destDesc;
		ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
		destDesc.Width = 128 * 4;
		destDesc.Height = 64;
		destDesc.MipLevels = 1; // 꼭 1로 하자 데이터 섞일 수 있음
		destDesc.ArraySize = 1; // 꼭 1로 해야함 데이터 섞일 수 있음
		destDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		destDesc.SampleDesc.Count = 1;
		destDesc.SampleDesc.Quality = 0;
		destDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		destDesc.Usage = D3D11_USAGE_DYNAMIC;
		destDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		HRESULT hr;
		hr = D3D::GetDevice()->CreateTexture2D(&destDesc, NULL, &transTexture);
		assert(SUCCEEDED(hr));

		CsResource::CreateSRV(transTexture, &transSrv);
	}

	// Map
	{
		D3DXMATRIX boneTransforms[128];

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
			transforms[i] = bone->Global() * boneTransforms[i];
		}

		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(transTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			for (int i = 0; i < 64; i++) {
				void * p = ((D3DXMATRIX*)subResource.pData) + 128 * i;
				memcpy(p, transforms, sizeof(D3DXMATRIX) * 128);
			}
		}
		D3D::GetDC()->Unmap(transTexture, 0);
	}
}

void ModelInstance::Update()
{
}

void ModelInstance::Render()
{
	ImGui::DragInt("Count", (int*)&maxCount, 1, 0, 64);

	for (ModelMesh* mesh : model->Meshes())
		mesh->RenderInstance(maxCount);
}
