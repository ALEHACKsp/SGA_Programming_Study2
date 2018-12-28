#pragma once

class AreaLight
{
public:
	AreaLight();
	~AreaLight();

	void Update();
	void Render();
	void Render(UINT index);

public:
	struct Desc
	{
		D3DXVECTOR3 Position;
		float Width;
		D3DXVECTOR3 Color;
		float Height;
		D3DXVECTOR3 Direction;
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
				Data.Lights[i].Width = 0;
				Data.Lights[i].Color = D3DXVECTOR3(0, 0, 0);
				Data.Lights[i].Height = 0;
				Data.Lights[i].Direction = D3DXVECTOR3(0, 0, 0);
				Data.Lights[i].Intensity = 0;
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
		buffer->Data.Lights[buffer->Data.Count] = desc;
		buffer->Data.Count++;
	}

	Desc* SetDesc(UINT index)
	{
		if (index >= buffer->Data.Count)
			return NULL;
		return &buffer->Data.Lights[index];
	}

	bool Remove(UINT index)
	{

	}
	//TODO: remove ¸¸µé±â
	UINT GetCount() { return buffer->Data.Count; }
	void SetCount(UINT count) { buffer->Data.Count = count; }
};