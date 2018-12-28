#pragma once

class Sharpening : public Render2D
{
public:
	Sharpening(ExecuteValues* values);
	~Sharpening();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Width = 1;
			Data.Height = 1;
			Data.Sharpen = 2.0f;
		}

		struct Struct
		{
			int Width;
			int Height;
			float Sharpen;
			float Padding;
		} Data;
	};
	Buffer* buffer;
};