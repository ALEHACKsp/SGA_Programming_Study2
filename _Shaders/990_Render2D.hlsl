#include "000_Header.hlsl"

// uv�� �ȼ� ���̴����� ó��
// ���ø� ����ϸ鼭 �������ֽǲ�

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

SamplerState Sampler : register(s10); // sampler�� ���� �ƴ϶� s0
// �⺻�� �ᵵ �� �ȳ־����� �⺻�� ����
Texture2D Map : register(t10); // texture�� t0

PixelInput VS(VertexTexture input)
{
    PixelInput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

	// �̷����ϸ� ������ ��
	// NDC ���� - dx9���� FVF_RHWXYZ ���Ŷ� ����
    //output.Position = input.Position;

    output.Uv = input.Uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.Uv); // �����ϳ� �������ֽǲ�

    return color;
}