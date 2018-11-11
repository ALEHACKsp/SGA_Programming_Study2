#include "000_Header2.hlsl"

struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : UV0;
    float3 Instance : INSTANCE0;
	// �̰� ���̵�� �޾���
    uint InstanceId : SV_INSTANCEID;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    uint id : INSTANCEID0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Position.x = input.Position.x + input.Instance.x;
    output.Position.y = input.Position.y + input.Instance.y;
    output.Position.z = input.Position.z + input.Instance.z;
    output.Position.w = 1;

    //output.Position.x = input.Position.x + input.InstanceId;
    //output.Position.y = input.Position.y + 0;
    //output.Position.z = input.Position.z + 0;

    output.Position = mul(output.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;
    output.id = input.InstanceId;

    return output;
}

Texture2D Map : register(t10);
SamplerState Sampler : register(s10);

float4 PS(VertexOutput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.Uv);

    return color;
}