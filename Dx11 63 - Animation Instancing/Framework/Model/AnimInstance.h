#pragma once

class AnimInstance
{
public:
	AnimInstance(class Model* model, class ModelClip* clip, wstring shaderFile);
	~AnimInstance();

	void AddWorld(D3DXMATRIX& world);

	void Ready();
	void Update();
	void Render();

private:
	class Model* model;

	UINT maxCount;
	D3DXMATRIX** transforms;

	D3DXMATRIX worlds[64];

	ID3D11Texture2D* transTexture;
	ID3D11ShaderResourceView* transSrv;

	vector<class ModelClip*> clips;

private:
	struct FrameDesc
	{
		UINT Curr;
		UINT Next;
		float Time;
		float FrameTime;
	};
	FrameDesc frames[64];
};
