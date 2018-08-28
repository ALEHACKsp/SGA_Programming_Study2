#pragma once

class PointLight
{
public:
	PointLight();
	~PointLight();

	void Update();
	void PostRender(int index);
	void DebugDraw(int index);
	int Count() { return buffer->Data.Count; }

private:
	class DebugDrawSphere * debug;

public:
	struct Desc
	{
		D3DXVECTOR3 Position;
		float Range;
		D3DXVECTOR3 Color;
		float Intensity;
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
				Data.Lights[i].Intensity = 0;
				Data.Lights[i].Range = 0;
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
		buffer->Data.Count--;
		if (buffer->Data.Count < 0)
			buffer->Data.Count = 0;
	}

	Desc* GetByIndex(int i)
	{
		return &buffer->Data.Lights[i];
	}

	Desc* GetBack()
	{
		if (buffer->Data.Count > 0)
			return &buffer->Data.Lights[buffer->Data.Count - 1];
		else
			return NULL;
	}

	// Remove 이런건 알아서 만들기
};