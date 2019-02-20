#pragma once

class AnimInstance
{
public:
	enum Mode { Mode_Play, Mode_Pause, Mode_Stop };

public:
	AnimInstance(class Model* model, wstring shaderFile);
	~AnimInstance();

	void AddWorld(D3DXMATRIX& world);
	void AddClip(class ModelClip* clip, float blendTime = 30);

	void Ready();
	void Update();
	void Render();

	void Play(int id = -1);
	void Pause(int id = -1);
	void Stop(int id = -1);

private:
	void LoadClip(wstring file);
	void SortTextureArray(); // Texture(clip) 삭제되었을시 다시 texturearray에 맵핑해서 add할때 문제 해결

private:
	void ImGuiRender();
	void ImGuiTotal();
	void ImGuiInstance();
	void ImGuiClipEdit(bool bInstance = false);

private:
	class Model* model;

	UINT maxCount;
	D3DXMATRIX** transforms;

	D3DXMATRIX* worlds;

	ID3D11Texture2D* textureArray;
	ID3D11ShaderResourceView* transSrv;

	vector<class ModelClip*> clips;

	int maxFrameCount;
	int textureArraySize;

	int instanceId;

private:
	struct FrameDesc
	{
		UINT Curr;
		UINT Next;
		float Time;
		float FrameTime;
		int ClipId;
		int NextClipId;
		float Temp[2];
	};
	FrameDesc* frames;

	float playRate;
	Mode playMode;
	int curFrame;

	int clipId;
	int nextClipId;
	vector<float> blendTimes;

	struct FrameDesc2
	{
		float PlayRate;
		Mode PlayMode;
	};
	FrameDesc2* frames2;
};
