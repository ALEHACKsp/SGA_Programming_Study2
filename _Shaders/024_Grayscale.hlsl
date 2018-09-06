#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

PixelInput VS(VertexTexture input)
{
    PixelInput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

	// �̷����ϸ� ������ ��
	// NDC ����
    //output.Position = input.Position;

    output.Uv = input.Uv;

    return output;
}

///////////////////////////////////////////////////////////////////////////////

cbuffer PS_Select : register(b10)
{
    int Select;
}

SamplerState Sampler : register(s10); // sampler�� ���� �ƴ϶� s0
// �⺻�� �ᵵ �� �ȳ־����� �⺻�� ����
Texture2D Map : register(t10); // texture�� t0

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.Uv); // �����ϳ� �������ֽǲ�
    
	[branch]
    if (Select == 0)
    {
		// ��ġ�� ȸ���� ����� ����� �������ٰ� �� Average
        color.rgb = (color.r + color.g + color.b) / 3.0f;
    }	
    else if (Select == 1)
    {
		// Grayscale // �����ڵ��� �����س��� ��ġ�� ����
        float3 monotone = float3(0.299f, 0.587f, 0.114f);
        color.rgb = dot(color.rgb, monotone);
    }
	else if (Select == 2)
    {
		// Inverse
		color.rgb = saturate(1.0f - color.rgb); 
    }

    return color;
}