#pragma once
#include "Execute.h"

class TestAmbient : public Execute
{
public:
	TestAmbient(ExecuteValues* values);
	~TestAmbient();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	class MeshPlane* plane;
	class MeshCube* cube;
	class MeshSphere* sphere;
	class MeshSphere* sphere2;
	class MeshBunny* bunny;

	Shader* shader;

private:
	enum class ELightType
	{
		None, Point, Spot, Capsule
	};

	struct LightDesc
	{
		ELightType Type;
		D3DXVECTOR3 Position;
		D3DXVECTOR3 Direction;
		float Range;
		float Length;
		float Outer;
		float Inner;
		D3DXVECTOR3 Color;
	};

	struct LightData
	{
		LightData()
		{
			PositionX = D3DXVECTOR4(0, 0, 0, 0);
			PositionY = D3DXVECTOR4(0, 0, 0, 0);
			PositionZ = D3DXVECTOR4(0, 0, 0, 0);

			DirectionX = D3DXVECTOR4(0, 0, 0, 0);
			DirectionY = D3DXVECTOR4(0, 0, 0, 0);
			DirectionZ = D3DXVECTOR4(0, 0, 0, 0);

			Range = D3DXVECTOR4(0, 0, 0, 0);

			SpotOuter = D3DXVECTOR4(-2.0f, -2.0f, -2.0f, -2.0f);
			SpotInner = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

			CapsuleLength = D3DXVECTOR4(0, 0, 0, 0);

			ColorR = D3DXVECTOR4(0, 0, 0, 0);
			ColorG = D3DXVECTOR4(0, 0, 0, 0);
			ColorB = D3DXVECTOR4(0, 0, 0, 0);
		}

		D3DXVECTOR4 PositionX;
		D3DXVECTOR4 PositionY;
		D3DXVECTOR4 PositionZ;

		D3DXVECTOR4 DirectionX;
		D3DXVECTOR4 DirectionY;
		D3DXVECTOR4 DirectionZ;

		D3DXVECTOR4 Range;

		D3DXVECTOR4 SpotOuter;
		D3DXVECTOR4 SpotInner;

		D3DXVECTOR4 CapsuleLength;

		D3DXVECTOR4 ColorR;
		D3DXVECTOR4 ColorG;
		D3DXVECTOR4 ColorB;

		void Setting(UINT index, LightDesc& desc)
		{
			PositionX[index] = desc.Position.x;
			PositionY[index] = desc.Position.y;
			PositionZ[index] = desc.Position.z;

			if (desc.Type == ELightType::Spot)
			{
				DirectionX[index] = -desc.Direction.x;
				DirectionY[index] = -desc.Direction.y;
				DirectionZ[index] = -desc.Direction.z;
			}
			else if (desc.Type == ELightType::Capsule)
			{
				DirectionX[index] = desc.Direction.x;
				DirectionY[index] = desc.Direction.y;
				DirectionZ[index] = desc.Direction.z;
			}
			
			Range[index] = 1.0f / desc.Range;

			// -2.0f 면 안쓰는거
			SpotOuter[index] = (desc.Type == ELightType::Spot) ? cosf(desc.Outer) : -2.0f;
			// 1.0f 면 안쓰는거
			SpotInner[index] = (desc.Type == ELightType::Spot) ? 1.0f / cosf(desc.Inner) : 1.0f;

			if (desc.Type == ELightType::Capsule)
				CapsuleLength[index] = desc.Length;

			ColorR[index] = desc.Color.x * desc.Color.x;
			ColorG[index] = desc.Color.y * desc.Color.y;
			ColorB[index] = desc.Color.z * desc.Color.z;
		}
	};

	class LightBuffer : public ShaderBuffer
	{
	public:
		LightBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			//ZeroMemory(Data.lights, sizeof(LightData) * 6);
			//for(int i=0; i<6; i++)
			//	Data.lights[i].

			Data.Count = 0;
		}

		void AddLight(LightData& data)
		{
			Data.lights[Data.Count] = data;
			Data.Count++;
		}

	private:
		struct Struct
		{
			LightData lights[6];

			int Count;
			float Padding[3];
		} Data;
	};

	LightBuffer * lightBuffer;
};