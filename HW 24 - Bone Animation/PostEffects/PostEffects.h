#pragma once

class PostEffects : public Render2D
{
public:
	PostEffects(ExecuteValues* values);
	~PostEffects();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Select = 0;
			Data.Count = 1;
			Data.Gamma = D3DXCOLOR(1, 1, 1, 1);
		}

		struct Struct
		{
			int Select;
			int Width;
			int Height;
			int Count;

			D3DXCOLOR Gamma;
		} Data;
	};

	Buffer* buffer;
};