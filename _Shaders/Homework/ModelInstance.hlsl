#include "../000_Header.hlsl"

cbuffer VS_BonesInstance : register(b4)
{
    matrix InstanceBones[1024];

    int InstanceUseBlend;
}

struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;

	int InstanceID : INSTANCE0;
};

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
};

matrix InstanceBoneWorld(int id)
{
    return InstanceBones[id];
}

PixelInput VS(VertexInput input)
{
    PixelInput output;

    matrix world = InstanceBoneWorld(input.InstanceID);
    //matrix world = BoneWorld();
    output.Position = mul(input.Position, world);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = WorldNormal(input.Normal, world);

    output.Uv = input.Uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    DiffuseLighting(color, input.Normal);

    //return float4(0, 0, input.InstanceID.x, 1);
    return color;

    //return float4(0, 0, 0, 1);
    // 디버깅 하기 위해서 normal을 반환형인 색상값으로
    //return float4((input.Normal * 0.5f) + 0.5f, 1);
}