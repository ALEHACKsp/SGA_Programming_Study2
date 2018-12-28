#pragma once

class ColorShift : public Render2D
{
public:
	ColorShift(ExecuteValues* values);
	~ColorShift();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(1, 1, 1, 1);
		}

		struct Struct
		{
			D3DXCOLOR Color;
		} Data;
	};
	Buffer* buffer;
};