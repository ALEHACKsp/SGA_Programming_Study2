#pragma once

class Sky
{
public:
	Sky(ExecuteValues* values);
	~Sky();

	void Update();
	void Render();

	void Set(D3DXCOLOR center, D3DXCOLOR apex, float height) {
		buffer->Data.Center = center;
		buffer->Data.Apex = apex;
		buffer->Data.Height = height;
	}

private:
	class GameModel* model;

	ExecuteValues* values;

	RasterizerState* cullMode[2];
	DepthStencilState* depthMode[2];
private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Center = D3DXCOLOR(0xFF0080FF);
			Data.Apex = D3DXCOLOR(0xFF9BCDFF);
			Data.Height = 4.5f;
		}

		struct Struct
		{
			D3DXCOLOR Center;
			D3DXCOLOR Apex;

			float Height;

			float Padding[3];
		} Data;
	};

	Buffer* buffer;
};