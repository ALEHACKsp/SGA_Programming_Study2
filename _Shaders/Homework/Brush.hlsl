#include "../000_Header.hlsl"

cbuffer VS_Brush : register(b10)
{
    int Type;
    float3 Location;

    int Range;
    float3 Color;
}

// location brush의 location
float3 BrushColor(float3 location)
{
    // 0이면 안찍힘
    if(Type == 0)
        return float3(0, 0, 0);

    // 웬만하면 else if 안쓰는게 좋다고 하심 나중에 설명해주실꺼
    if (Type == 1)
    {
        if ((location.x >= (Location.x - Range)) &&
            (location.x <= (Location.x + Range)) &&
            (location.z >= (Location.z - Range)) &&
            (location.z <= (Location.z + Range)))
        {
            return Color;
        }
    }

    if (Type == 2)
    {
        float dx = location.x - Location.x;
        float dz = location.z - Location.z;

        float dist = sqrt(dx * dx + dz * dz);

        if (dist <= Range)
            return Color;
    }

    if (Type == 3)
    {
        float dx = location.x - Location.x;
        float dz = location.z - Location.z;

        float dist = sqrt(dx * dx + dz * dz);

        if (dist <= Range)
            return Color;
    }

    return float3(0, 0, 0);
}

struct PixelInput
{
    float4 Position : SV_POSITION; // SV만 정해진 예약어 나머진 편한대로 쓰면 됨
    float2 Uv : UV0;
    float3 Normal : NORMAL0;

    float3 BrushColor : COLOR0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    // 사실 월드 변환된 이후에 position에 따라 해야되긴 함
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);

    // oPosition 씀 
    output.BrushColor = BrushColor(input.Position.xyz);

    output.Uv = input.Uv;

    return output;
}

Texture2D ColorMap : register(t10);
SamplerState ColorSampler : register(s10);

Texture2D ColorMap2 : register(t11);
SamplerState ColorSampler2 : register(s11);

Texture2D AlphaMap : register(t12);
SamplerState AlphaSampler : register(s12);

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    float4 colorMap = ColorMap.Sample(ColorSampler, input.Uv);
    float4 colorMap2 = ColorMap2.Sample(ColorSampler2, input.Uv);
    float4 alphaMap = AlphaMap.Sample(AlphaSampler, input.Uv);

    float4 alpha = float4(alphaMap.r, alphaMap.r, alphaMap.r, alphaMap.r);
    float4 temp = lerp(colorMap, colorMap2, alpha);

    DiffuseLighting(color, temp, input.Normal);

    color = color + float4(input.BrushColor, 0);

    return color;
    // 디버깅 하기 위해서 normal을 반환형인 색상값으로
    //return float4((input.Normal * 0.5f) + 0.5f, 1);
}