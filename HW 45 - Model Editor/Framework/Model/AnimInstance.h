#pragma once

class AnimInstance
{
public:
	enum Mode { Mode_Play, Mode_Pause, Mode_Stop };

public:
	AnimInstance(class Model* model, class ModelClip* clip, wstring shaderFile);
	AnimInstance(class Model* model, vector<ModelClip*>& clips, wstring shaderFile, vector<string>& clipNames);
	~AnimInstance();

	void AddWorld(D3DXMATRIX& world);

	void Ready();
	void Update();
	void Render();

private:
	class Model* model;

	UINT maxCount;
	D3DXMATRIX** transforms;

	D3DXMATRIX* worlds;

	ID3D11Texture2D* textureArray;
	ID3D11ShaderResourceView* transSrv;

	vector<ID3D11Texture2D*> transTextures;

	vector<class ModelClip*> clips;
	vector<string> clipNames;

	int maxFrameCount;
	priority_queue<int> pQueue;

private:
	struct FrameDesc
	{
		UINT Curr;
		UINT Next;
		float Time;
		float FrameTime;
	};
	FrameDesc* frames;

	float playRate;
	int clipId;

	struct FrameDesc2
	{
		float PlayRate;
		Mode PlayMode;
	};
	FrameDesc2* frames2;
};
