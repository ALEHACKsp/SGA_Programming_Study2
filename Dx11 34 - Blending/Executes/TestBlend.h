#pragma once
#include "Execute.h"

class TestBlend : public Execute
{
public:
	TestBlend(ExecuteValues* values);
	~TestBlend();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	Shader* shader;
	class MeshQuad* quad;
	class MeshQuad* quad2;

	BlendState* blendState[10];

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Selected = 0;
		}

		struct Struct
		{
			int Selected;

			float Padding[3];
		} Data;
	};

	Buffer* buffer;
};