#pragma once

class Gamma : public Render2D
{
public:
	Gamma(ExecuteValues* values);
	~Gamma();

	void Render();

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.R = 1;
			Data.G = 1;
			Data.B = 1;
		}

		struct Struct
		{
			float R;
			float G;
			float B;
			float Padding;
		} Data;
	};
	Buffer* buffer;
};