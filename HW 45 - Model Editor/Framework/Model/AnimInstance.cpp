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
	frames = new FrameDesc[64];
	worlds = new D3DXMATRIX[64];
	frames2 = new FrameDesc2[64];

	for (Material* material : model->Materials())
		material->SetShader(shaderFile);

	clips.push_back(clip);
	maxFrameCount = clip->FrameCount();

	ZeroMemory(frames, sizeof(FrameDesc) * 64);

	playRate = 1.0f;
	clipId = 0;

	for (int i = 0; i < 64; i++) {
		frames2[i].PlayMode = Mode::Mode_Stop;
		frames2[i].PlayRate = playRate;
	}
}

AnimInstance::AnimInstance(Model * model, vector<ModelClip*>& clips, wstring shaderFile, vector<string>& clipNames)
	: maxCount(0), model(model)
{
	frames = new FrameDesc[64];
	worlds = new D3DXMATRIX[64];
	frames2 = new FrameDesc2[64];

	for (Material* material : model->Materials())
		material->SetShader(shaderFile);

	this->clips = clips;
	this->clipNames = clipNames;

	for (ModelClip* clip : clips)
		pQueue.push(clip->FrameCount());
	maxFrameCount = pQueue.top();

	ZeroMemory(frames, sizeof(FrameDesc) * 64);

	playRate = 1.0f;
	clipId = 0;

	for (int i = 0; i < 64; i++) {
		frames2[i].PlayMode = Mode::Mode_Stop;
		frames2[i].PlayRate = playRate;
	}
}

AnimInstance::~AnimInstance()
{
	SAFE_DELETE_ARRAY(worlds);
	SAFE_DELETE_ARRAY(frames);
	SAFE_DELETE_ARRAY(frames2);

	SAFE_RELEASE(textureArray)
	SAFE_RELEASE(transSrv);

	for (ID3D11Texture2D* transTexture : transTextures)
		SAFE_RELEASE(transTexture);
}

void AnimInstance::Ready()
{


	// Textures
	for (int j = 0; j < clips.size(); j++)
	{
		ID3D11Texture2D* transTexture;
		// Texture
		{
			D3D11_TEXTURE2D_DESC destDesc;
			ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
			destDesc.Width = 128 * 4;
			destDesc.Height = maxFrameCount;
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
		}

		// Map
		{
			transforms = new D3DXMATRIX*[maxFrameCount];

			for (int i = 0; i < maxFrameCount; i++)
				transforms[i] = new D3DXMATRIX[128];

			D3DXMATRIX boneTransforms[128];

			for (UINT c = 0; c < clips[j]->FrameCount(); c++)
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
					ModelKeyframe * frame = clips[j]->Keyframe(bone->Name());

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
				for (int i = 0; i < clips[j]->FrameCount(); i++) {
					void * p = ((D3DXMATRIX*)subResource.pData) + i * 128;
					memcpy(p, transforms[i], sizeof(D3DXMATRIX) * 128);
				}
			}
			D3D::GetDC()->Unmap(transTexture, 0);

			for (int i = 0; i < maxFrameCount; i++)
				SAFE_DELETE_ARRAY(transforms[i]);

			SAFE_DELETE_ARRAY(transforms);
		}

		// Copy Texture
		{
			D3D11_TEXTURE2D_DESC srcDesc;
			transTexture->GetDesc(&srcDesc);

			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
			desc.Width = srcDesc.Width;
			desc.Height = srcDesc.Height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = srcDesc.Format;
			desc.SampleDesc = srcDesc.SampleDesc;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.Usage = D3D11_USAGE_STAGING;

			HRESULT hr;

			ID3D11Texture2D* texture;
			hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture);
			assert(SUCCEEDED(hr));

			hr = D3DX11LoadTextureFromTexture(D3D::GetDC(), transTexture, NULL, texture);
			assert(SUCCEEDED(hr));

			SAFE_RELEASE(transTexture);

			transTextures.push_back(texture);
		}
	}

	// Texture Array
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		transTextures[0]->GetDesc(&textureDesc);

		//Texture2DArray 생성
		{
			D3D11_TEXTURE2D_DESC desc;
			desc.Width = textureDesc.Width;
			desc.Height = textureDesc.Height;
			desc.MipLevels = textureDesc.MipLevels;
			desc.ArraySize = transTextures.size();
			desc.Format = textureDesc.Format;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &textureArray);
			assert(SUCCEEDED(hr));
		}


		for (UINT i = 0; i < transTextures.size(); i++)
		{
			for (UINT level = 0; level < textureDesc.MipLevels; level++)
			{
				D3D11_MAPPED_SUBRESOURCE subResource;
				D3D::GetDC()->Map(transTextures[i], level, D3D11_MAP_READ, 0, &subResource);
				{
					D3D::GetDC()->UpdateSubresource(textureArray, D3D11CalcSubresource(level, i, textureDesc.MipLevels), NULL, subResource.pData, subResource.RowPitch, subResource.DepthPitch);
				}
				D3D::GetDC()->Unmap(transTextures[i], level);
			}
		}

		//SRV 생성
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			desc.Format = textureDesc.Format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.MostDetailedMip = 0;
			desc.Texture2DArray.MipLevels = textureDesc.MipLevels;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.ArraySize = transTextures.size();

			HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(textureArray, &desc, &transSrv);
			assert(SUCCEEDED(hr));
		}
	}

	for (Material* material : model->Materials()) {
		material->GetShader()->AsSRV("Transforms")->SetResource(transSrv);
		material->GetShader()->AsScalar("ClipId")->SetInt(clipId);
	}
}

void AnimInstance::Update()
{
	for (int i = 0; i < maxCount; i++)
	{
		frames[i].FrameTime += Time::Delta();

		ModelClip* clip = clips[clipId];

		float invFrameRate = 1.0f / clip->FrameRate();

		if (frames[i].FrameTime > (invFrameRate / playRate))
		{
			frames[i].FrameTime = 0;

			frames[i].Curr = (frames[i].Curr + 1) % clip->FrameCount();
			frames[i].Next = (frames[i].Curr + 1) % clip->FrameCount();
		}

		frames[i].Time = frames[i].FrameTime / (invFrameRate / playRate);
	}

	for (Material* material : model->Materials())
		material->GetShader()->Variable("Frames")->SetRawValue(frames, 0, sizeof(FrameDesc) * 64);
}

void AnimInstance::Render()
{
	ImGui::Begin("Anim Instance");

	ImGui::Text("Clip : %s", clipNames[clipId].c_str());
	ImGui::Text("FrameCount : %d", clips[clipId]->FrameCount());

	ImGui::DragInt("Count", (int*)&maxCount, 1, 0, 64);
	if (ImGui::InputInt("ClipId", &clipId, 1)) {
		if (clipId < 0) clipId = 0;
		if (clipId >= clips.size()) clipId = clips.size() - 1;

		for (Material* material : model->Materials())
			material->GetShader()->AsScalar("ClipId")->SetInt(clipId);
	}

	if (ImGui::DragFloat("##totalSpeed", &playRate, 0.1f)) {
		if (playRate <= 0.0f) playRate = 0;

		for (int i = 0; i < maxCount; i++) frames2[i].PlayRate = playRate;
	}

	if (ImGui::TreeNode("Childs")) {
		ImGui::BeginChild("Instance");

		for (int i = 0; i < maxCount; i++) {

			if (ImGui::DragFloat(("##Speed" + to_string(i)).c_str(), &frames2[i].PlayRate, 0.1f))
				if (frames2[i].PlayRate <= 0.0f) frames2[i].PlayRate = 0;

		}
		ImGui::EndChild();

		ImGui::TreePop();
	}

	ImGui::End();

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
}
