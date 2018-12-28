#pragma once

class Shadow
{
public:
	Shadow(ExecuteValues* values);
	~Shadow();

	void Add(class GameModel* obj);
	
	void Update();
	void PreRender();
	void Render();

private:
	ExecuteValues* values;

	Shader* shaderDepth;
	RenderTarget* depthShadow;
	Render2D* depthRender;

	Shader* shaderRender;

	vector<class GameModel*> objs;
	
	class Perspective* perspective;
	ViewProjectionBuffer* vpBuffer;
	SamplerState* samplerState[2];

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Size = D3DXVECTOR2(1, 1);
			Data.Bias = 0.0005f;
			Data.Selected = 0;
		}

		struct Struct
		{
			D3DXVECTOR2 Size;
			float Bias;
			int Selected;
		} Data;
	};

	Buffer* buffer;
};
