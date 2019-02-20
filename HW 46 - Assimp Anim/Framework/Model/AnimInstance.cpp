#include "FrameWork.h"
#include "AnimInstance.h"

#include "Model.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"
#include "ModelBone.h"

#include "ModelClip.h"

AnimInstance::AnimInstance(Model * model, wstring shaderFile)
	: maxCount(0), model(model)
	, playRate(1.0f), clipId(0), nextClipId(0)
	, maxFrameCount(512), textureArraySize(8), instanceId(0)
	, playMode(Mode::Mode_Play), curFrame(0)
{
	frames = new FrameDesc[64];
	worlds = new D3DXMATRIX[64];
	frames2 = new FrameDesc2[64];

	for (Material* material : model->Materials())
		material->SetShader(shaderFile);

	for (int i = 0; i < 64; i++) {
		frames[i].Curr = 0;
		frames[i].Next = 1;
		frames[i].FrameTime = 0;
		frames[i].Time = 0;
		frames[i].ClipId = 0;
		frames[i].NextClipId = 0;

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

	for (ModelClip* clip : clips)
		SAFE_DELETE(clip);
}

void AnimInstance::Ready()
{
	// Create Texture Array
	{
		//Texture2DArray 생성
		{
			D3D11_TEXTURE2D_DESC desc;
			desc.Width = 128 * 4;
			desc.Height = maxFrameCount;
			desc.MipLevels = 1;
			desc.ArraySize = textureArraySize;
			desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			HRESULT hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &textureArray);
			assert(SUCCEEDED(hr));
		}

		//SRV 생성
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			desc.Texture2DArray.MostDetailedMip = 0;
			desc.Texture2DArray.MipLevels = 1;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.ArraySize = textureArraySize;

			HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(textureArray, &desc, &transSrv);
			assert(SUCCEEDED(hr));
		}
	}

	for (Material* material : model->Materials()) {
		material->GetShader()->AsSRV("Transforms")->SetResource(transSrv);
	}
}

void AnimInstance::Update()
{
	if (playMode != Mode::Mode_Play) return;

	for (int i = 0; i < maxCount; i++)
	{
		if (frames2[i].PlayMode != Mode::Mode_Play) continue;

		frames[i].FrameTime += Time::Delta();

		if (frames[i].ClipId == frames[i].NextClipId) {
			ModelClip* clip = clips[frames[i].ClipId];

			float invFrameRate = 1.0f / clip->FrameRate();

			if (frames[i].FrameTime > (invFrameRate / frames2[i].PlayRate))
			{
				frames[i].FrameTime = 0;

				frames[i].Curr = frames[i].Next;
				frames[i].Next++;
				// Next가 마지막 프레임까지 온거면 마지막 프레임으로 유지 (Repeat 아닌 경우 처리)
				if (frames[i].Next >= clip->FrameCount())
					frames[i].Next = clip->FrameCount() - 1;

				// 마지막 프레임인 경우 한번 더 다음 프레임으로 가줘야함 두 프레임간에 보간이므로 
				// 마지막 프레임과 처음 프레임이 똑같을 수 있음
				// Repeat이면 이렇게
				if (clip->Repeat() && frames[i].Curr == clip->FrameCount() - 1) {
					frames[i].Curr = 0;
					frames[i].Next = 1;
				}
			}

			frames[i].Time = frames[i].FrameTime / (invFrameRate / frames2[i].PlayRate);
		}
		else {
			// blend 값이 너무 낮아서 곱한거
			float t = (frames[i].FrameTime * 100.0f) / blendTimes[frames[i].ClipId];

			if (t > 1.0f) {
				clipId = nextClipId;
				frames[i].ClipId = frames[i].NextClipId;

				frames[i].Curr = 0;
				frames[i].Next = 1;
				frames[i].FrameTime = 0;
				frames[i].Time = 0;
			}
			else
				frames[i].Time = t;
		}
	}

	for (Material* material : model->Materials())
		material->GetShader()->Variable("Frames")->SetRawValue(frames, 0, sizeof(FrameDesc) * 64);
}

void AnimInstance::Render()
{
	ImGuiRender();

	for (ModelMesh* mesh : model->Meshes())
		mesh->RenderInstance(maxCount);
}

void AnimInstance::Play(int id)
{
	// 전체 변경
	if (id == -1) {
		playMode = Mode::Mode_Play;
		for (int i = 0; i < maxCount; i++)
			frames2[i].PlayMode = Mode::Mode_Play;
	}
	else {
		playMode = Mode::Mode_Play;
		frames2[id].PlayMode = Mode::Mode_Play;
	}
}

void AnimInstance::Pause(int id)
{
	// 전체 변경
	if (id == -1) {
		for (int i = 0; i < maxCount; i++)
			frames2[i].PlayMode = Mode::Mode_Pause;
	}
	else {
		frames2[id].PlayMode = Mode::Mode_Pause;
	}
}

void AnimInstance::Stop(int id)
{
	// 전체 변경
	if (id == -1) {
		for (int i = 0; i < maxCount; i++) {
			frames2[i].PlayMode = Mode::Mode_Stop;
			frames[i].Curr = 0;
			frames[i].Next = 1;
			frames[i].FrameTime = 0;
			frames[i].Time = 0;
		}
	}
	else {
		frames2[id].PlayMode = Mode::Mode_Stop;
		frames[id].Curr = 0;
		frames[id].Next = 1;
		frames[id].FrameTime = 0;
		frames[id].Time = 0;

	}
}

void AnimInstance::LoadClip(wstring file)
{
	AddClip(new ModelClip(file));
}

void AnimInstance::SortTextureArray()
{
	for (int i = 0; i < clips.size(); i++) {
		ModelClip* clip = clips[i];
		// Texture Array의 Map
		{
			D3D11_MAPPED_SUBRESOURCE subResource;
			D3D::GetDC()->Map(clip->Texture(), 0, D3D11_MAP_READ, 0, &subResource);
			{
				D3D::GetDC()->UpdateSubresource(textureArray, D3D11CalcSubresource(0, i, 1), NULL, subResource.pData, subResource.RowPitch, subResource.DepthPitch);
			}
			D3D::GetDC()->Unmap(clip->Texture(), 0);
		}
	}
}

void AnimInstance::ImGuiRender()
{
	if (maxCount > 0) {
		ImGuiTotal();
		ImGuiInstance();
	}
}

void AnimInstance::ImGuiTotal()
{
	ImGui::Begin("Anim Instance");

	ImGuiClipEdit();

	ImGui::Columns(3, 0, false);
	float width = ImGui::GetWindowSize().x / 3.5f;
	if (ImGui::Button("Play##total", ImVec2(width, 0))) Play();
	ImGui::NextColumn();
	if (ImGui::Button("Pause##total", ImVec2(width, 0))) Pause();
	ImGui::NextColumn();
	if (ImGui::Button("Stop##total", ImVec2(width, 0))) Stop();
	ImGui::Columns();

	curFrame = frames[0].Curr;
	if (ImGui::SliderInt("Frame##curFrame", &curFrame, 0, clips[clipId]->FrameCount() - 2)) {
		playMode = Mode::Mode_Play;
		for (int i = 0; i < maxCount; i++) {
			frames[i].Curr = curFrame;
			if (curFrame > clips[clipId]->FrameCount() - 2)
				curFrame = clips[clipId]->FrameCount() - 2;
			frames[i].Next = curFrame + 1;
			frames[i].FrameTime = 0;
			frames[i].Time = 0;
		}
	}

	if (ImGui::DragFloat("Speed##total", &playRate, 0.1f)) {
		if (playRate <= 0.0f) playRate = 0;

		for (int i = 0; i < maxCount; i++) frames2[i].PlayRate = playRate;
	}

	ImGui::End();
}

void AnimInstance::ImGuiInstance()
{

	ImGui::Begin("Instance##Instance");

	ImGuiClipEdit(true);

	ImGui::Columns(3);
	float width = ImGui::GetWindowSize().x / 3.5f;
	if (ImGui::Button("Play##instance", ImVec2(width, 0))) Play(instanceId);
	ImGui::NextColumn();
	if (ImGui::Button("Pause##instance", ImVec2(width, 0))) Pause(instanceId);
	ImGui::NextColumn();
	if (ImGui::Button("Stop##instance", ImVec2(width, 0))) Stop(instanceId);
	ImGui::Columns();

	if (ImGui::InputInt("ID", &instanceId, 1)) {
		if (instanceId < 0) instanceId = 0;
		if (instanceId > maxCount) instanceId = maxCount;
	}

	ImGui::Columns(2);
	if (ImGui::SliderInt("##Frame##instance", (int*)&frames[instanceId].Curr, 0, clips[clipId]->FrameCount() - 2)) {
		if (frames[instanceId].Curr > clips[clipId]->FrameCount() - 2)
			frames[instanceId].Curr = clips[clipId]->FrameCount() - 2;
		frames[instanceId].Next = frames[instanceId].Curr + 1;
		frames[instanceId].FrameTime = 0;
		frames[instanceId].Time = 0;
	}
	ImGui::NextColumn();
	if (ImGui::DragFloat(("##Speed##instance" + to_string(instanceId)).c_str(), &frames2[instanceId].PlayRate, 0.1f))
		if (frames2[instanceId].PlayRate <= 0.0f) frames2[instanceId].PlayRate = 0;
	ImGui::Columns();

	ImGui::End();
}

void AnimInstance::ImGuiClipEdit(bool bInstance)
{
	// Clips
	{
		ImVec2 base_pos = ImGui::GetCursorScreenPos();
		ImVec2 childSize = ImVec2(0, 160);

		if(bInstance == false)
			ImGui::BeginChild("Clips##child", childSize, true, ImGuiWindowFlags_MenuBar);
		else
			ImGui::BeginChild("Clips##instancechild", childSize, true, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar()) {
			if (bInstance == false) {
				if (ImGui::BeginMenu("Clips##menu")) {

					if (ImGui::MenuItem("Add Clip", "Add Animation Clip")) {
						function<void(wstring)> func = bind(&AnimInstance::LoadClip, this, placeholders::_1);
						Path::OpenFileDialog(L"", L"Animation Clip(*.animation)\0*.animation\0", Models, func);
					}
					ImGui::Separator();

					if (ImGui::TreeNodeEx("Delete Clip", ImGuiTreeNodeFlags_DefaultOpen)) {
						vector<ModelClip*>::iterator iter = clips.begin();

						int index = 0;
						for (; iter != clips.end();) {
							string name = (*iter)->ClipName() + "##delete";
							if (ImGui::MenuItem(name.c_str())) {
								SAFE_DELETE(*iter);
								iter = clips.erase(iter);
								blendTimes.erase(blendTimes.begin() + index);

								SortTextureArray();
							}
							else {
								iter++;
								index++;
							}
						}

						ImGui::TreePop();
					}

					ImGui::EndMenu();
				}
			}
			else {
				if (ImGui::BeginMenu("Clips##instance", false)) {
					ImGui::EndMenu();
				}
			}
			ImGui::EndMenuBar();
		}

		for (int i = 0; i < clips.size(); i++) {
			ImGui::BeginGroup();

			ImGui::Columns(2, 0, false);

			ImGui::BulletText("%s", clips[i]->ClipName().c_str());
			ImGui::NextColumn();

			ImGui::Text("Frame: %d", clips[i]->FrameCount());
			ImGui::NextColumn();

			if (ImGui::DragFloat(("Time##BlendTimeclip" + to_string(i)).c_str(), &blendTimes[i], 0.1f, 1, 100, "%.1f"))
				if (blendTimes[i] <= 0) blendTimes[i] = 1.0f;
			ImGui::NextColumn();

			ImGui::Checkbox(("Repeat##clip" + to_string(i)).c_str(), clips[i]->Repeat());

			ImGui::Columns();

			ImGui::EndGroup();

			ImColor itemColor;
			if (bInstance == true) {
				if (ImGui::IsItemClicked()) {
					frames[instanceId].NextClipId = i;
					if (frames[instanceId].ClipId != frames[i].NextClipId) {
						frames[instanceId].Next = 0;
						frames[instanceId].FrameTime = 0;
						frames[instanceId].Time = 0;
					}
				}
				itemColor = frames[instanceId].NextClipId == i ? ImColor(0.3f, 0.7f, 0.3f, 0.5f) : ImColor(0.2f, 0.2f, 0.7f, 0.5f);
			}
			else {
				if (ImGui::IsItemClicked()) {
					nextClipId = i;
					if (clipId != nextClipId) {
						for (int i = 0; i < maxCount; i++) {
							frames[i].Next = 0;
							frames[i].FrameTime = 0;
							frames[i].Time = 0;
							frames[i].NextClipId = nextClipId;
						}
					}
				}
				itemColor = frames[0].NextClipId == i ? ImColor(0.3f, 0.7f, 0.3f, 0.5f) : ImColor(0.2f, 0.2f, 0.7f, 0.5f);
			}


			// Test
			//ImVec2 pos = ImGui::GetWindowPos();
			//ImVec2 size = ImGui::GetWindowSize();
			//ImVec2 pos2 = ImGui::GetCursorScreenPos();
			//ImVec2 size2 = ImGui::GetItemRectSize();
			//ImVec2 pos3 = ImGui::GetCursorPos();

			ImVec2 item_rect_min = ImGui::GetItemRectMin();
			ImVec2 item_rect_max = ImGui::GetItemRectMax();
			//item_rect_min.x += base_pos.x;
			//item_rect_min.y += base_pos.y;

			item_rect_max.x += base_pos.x;
			//item_rect_max.y += base_pos.y;

			ImGui::GetWindowDrawList()->AddRectFilled(item_rect_min, item_rect_max, itemColor);
		}

		ImGui::EndChild();
	}
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

void AnimInstance::AddClip(ModelClip * clip, float blendTime)
{
	clips.push_back(clip);
	blendTimes.push_back(blendTime);

	// clip에 들어간 번호
	int index = clips.size() - 1;

	ID3D11Texture2D* transTexture;
	ID3D11Texture2D* copyTexture;

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

		for (UINT c = 0; c < clip->FrameCount(); c++)
		{
			for (UINT b = 0; b < model->BoneCount(); b++)
			{
				ModelBone * bone = model->BoneByIndex(b);

				D3DXMATRIX parentTransform;
				D3DXMATRIX invGlobal = bone->Transform();
				D3DXMatrixInverse(&invGlobal, NULL, &invGlobal);

				int parentIndex = bone->ParentIndex();
				if (parentIndex < 0)
					D3DXMatrixIdentity(&parentTransform);
				else
					parentTransform = boneTransforms[parentIndex];

				D3DXMATRIX animation;
				ModelKeyframe * frame = clip->Keyframe(bone->Name());

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
			for (int i = 0; i < clip->FrameCount(); i++) {
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

		hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &copyTexture);
		assert(SUCCEEDED(hr));

		hr = D3DX11LoadTextureFromTexture(D3D::GetDC(), transTexture, NULL, copyTexture);
		assert(SUCCEEDED(hr));

		SAFE_RELEASE(transTexture);

		clip->Texture(copyTexture);
	}

	// Texture Array의 Map
	{
		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(clip->Texture(), 0, D3D11_MAP_READ, 0, &subResource);
		{
			D3D::GetDC()->UpdateSubresource(textureArray, D3D11CalcSubresource(0, index, 1), NULL, subResource.pData, subResource.RowPitch, subResource.DepthPitch);
		}
		D3D::GetDC()->Unmap(clip->Texture(), 0);
	}
}
