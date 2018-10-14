#pragma once

class Shadow
{
//public:
//	// 높을 수록 정밀해지고 느려짐 보통 2^n으로 잡음
//	// 가장 좋은 건 화면 곱하기 2
//	const float Width = 2048;
//	const float Height = 2048;

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

	vector<class GameModel *> objs;

	class Perspective* perspective;
	ViewProjectionBuffer* vpBuffer;

	SamplerState* samplerState[2];

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Bias = 0.0005f;
			Data.Selected = 0;
		}

		struct Struct
		{
			float Bias;
			int Selected;

			float Padding[2];
		} Data;
	};

	Buffer* buffer;
};