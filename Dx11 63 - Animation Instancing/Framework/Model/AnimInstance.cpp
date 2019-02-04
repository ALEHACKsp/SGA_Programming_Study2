#include "FrameWork.h"
#include "AnimInstance.h"

#include "Model.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"
#include "ModelBone.h"

#include "ModelClip.h"

AnimInstance::AnimInstance(Model * model, ModelClip* clip, wstring shaderFile)
	: maxCount(0), model(model)
{
	for (Material* material : model->Materials())
		material->SetShader(shaderFile);

	clips.push_back(clip);

	ZeroMemory(frames, sizeof(FrameDesc) * 64);
}

AnimInstance::~AnimInstance()
{
	for (int i = 0; i < clips[0]->FrameCount(); i++)
		SAFE_DELETE_ARRAY(transforms[i]);

	SAFE_DELETE_ARRAY(transforms);

	SAFE_RELEASE(transTexture);
	SAFE_RELEASE(transSrv);
}

void AnimInstance::Ready()
{
	// Todo: clips[0]->FrameCount() 이거 최대 값 찾아야함

	{
		D3D11_TEXTURE2D_DESC destDesc;
		ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
		destDesc.Width = 128 * 4;
		destDesc.Height = clips[0]->FrameCount();
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
		transforms = new D3DXMATRIX*[clips[0]->FrameCount()];

		for (int i = 0; i < clips[0]->FrameCount(); i++)
			transforms[i] = new D3DXMATRIX[128];

		D3DXMATRIX boneTransforms[128];

		for (UINT c = 0; c < clips[0]->FrameCount(); c++)
		{
			for (UINT b = 0; b < model->BoneCount(); b++)
			{
				ModelBone * bone = model->BoneByIndex(b);

				D3DXMATRIX parentTransform;
				D3DXMATRIX invGlobal = bone->Global();
				D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);

				int parentIndex = bone->ParentIndex();
				if (parentIndex < 0)
					D3DXMatrixIdentity(&parentTransform);
				else
					parentTransform = boneTransforms[parentIndex];

				D3DXMATRIX animation;
				ModelKeyframe * frame = clips[0]->Keyframe(bone->Name());

				if (frame != NULL)
				{
					ModelKeyframeData data = frame->Transforms[c];

					D3DXMATRIX S, R, T;
					D3DXMatrixScaling(&S, data.Scale.x, data.Scale.y, data.Scale.z);
					D3DXMatrixRotationQuaternion(&R, &data.Rotation);
					D3DXMatrixTranslation(&T, data.Translation.x, data.Translation.y, data.Translation.z);

					animation = S * R * T;
				}
				else
				{
					D3DXMatrixIdentity(&animation);
				}

				boneTransforms[b] = animation * parentTransform;
				transforms[c][b] = invGlobal * boneTransforms[b];
			}
		}

		// 이건 usage dynamic일때만 가능
		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(transTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			for (int i = 0; i < clips[0]->FrameCount(); i++) {
				void * p = ((D3DXMATRIX*)subResource.pData) + i * 128;
				memcpy(p, transforms[i], sizeof(D3DXMATRIX) * 128);
			}
		}
		D3D::GetDC()->Unmap(transTexture, 0);
	}


}

void AnimInstance::Update()
{
	for (int i = 0; i < maxCount; i++)
	{
		frames[i].FrameTime += Time::Delta();

		ModelClip* clip = clips[0];

		float invFrameRate = 1.0f / clip->FrameRate();

		if (frames[i].FrameTime > invFrameRate)
		{
			frames[i].FrameTime = 0.0f;

			frames[i].Curr = (frames[i].Curr + 1) % clip->FrameCount();
			frames[i].Next = (frames[i].Curr + 1) % clip->FrameCount();
		}

		frames[i].Time = frames[i].FrameTime / invFrameRate;
	}

	for (Material* material : model->Materials())
		material->GetShader()->Variable("Frames")->SetRawValue(frames, 0, sizeof(FrameDesc) * 64);
}

void AnimInstance::Render()
{
	ImGui::DragInt("Count", (int*)&maxCount, 1, 0, 64);

	for (ModelMesh* mesh : model->Meshes())
		mesh->RenderInstance(maxCount);
}


void AnimInstance::AddWorld(D3DXMATRIX& world)
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

	for (Material* material : model->Materials())
		material->GetShader()->AsSRV("Transforms")->SetResource(transSrv);
}
