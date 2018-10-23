#include "../000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

Texture2D MoonMap : register(t10);
SamplerState MoonSampler : register(s10);

PixelInput VS(VertexTexture input)
{
    PixelInput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    return output;
}

cbuffer PS_Moon : register(b10)
{
    float Alpha;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = MoonMap.Sample(MoonSampler, input.Uv);
    color.a *= Alpha;

    return color;

    //return float4(0, 0, 0, 1);
    // 디버깅 하기 위해서 normal을 반환형인 색상값으로
    //return float4((input.Normal * 0.5f) + 0.5f, 1);
}