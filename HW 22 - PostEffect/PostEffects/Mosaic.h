#pragma once

class Mosaic : public Render2D
{
public:
	Mosaic(ExecuteValues* values);
	~Mosaic();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Select = 0;
			Data.Count = 1;
		}

		struct Struct
		{
			int Select;
			int Width;
			int Height;
			int Count;
		} Data;
	};

	Buffer* buffer;
};