#pragma once

class Embossing : public Render2D
{
public:
	Embossing(ExecuteValues* values);
	~Embossing();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Width = 1;
			Data.Height = 1;
			Data.PixelOffset = 1;
		}

		struct Struct
		{
			int Width;
			int Height;
			int PixelOffset;
			float Padding;
		} Data;
	};
	Buffer* buffer;
};