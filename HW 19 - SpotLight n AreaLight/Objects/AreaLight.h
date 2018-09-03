#pragma once

class AreaLight
{
public:
	AreaLight();
	~AreaLight();

	void Update();
	void PostRender(int index);
	void DebugDraw(int index);
	int Count() { return buffer->Data.Count; }

private:
	class DebugDrawCube * debug;

public:
	struct Desc
	{
		D3DXVECTOR3 Position;
		float Padding1;

		D3DXVECTOR3 Color;
		float Padding2;

		D3DXVECTOR3 HalfSize;
		float Padding3;
	};

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			for (int i = 0; i < 32; i++)
			{
				Data.Lights[i].Position = D3DXVECTOR3(0, 0, 0);
				Data.Lights[i].Color = D3DXVECTOR3(0, 0, 0);
				Data.Lights[i].HalfSize = D3DXVECTOR3(0, 0, 0);
			}

			Data.Count = 0;
		}

		struct Struct
		{
			Desc Lights[32];

			UINT Count;
			float Padding[3];
		} Data;
	};

	Buffer* buffer;

public:
	void Add(Desc& desc)
	{
		int count = buffer->Data.Count;

		buffer->Data.Lights[count] = desc;
		
		buffer->Data.Count++;
	}

	void LastDelete() {
		if (buffer->Data.Count > 0)
			buffer->Data.Count--;
	}

	// Remove 이런건 알아서 만들기
};