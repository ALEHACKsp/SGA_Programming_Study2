#include "../000_Header.hlsl"

cbuffer VS_Brush : register(b10)
{
    int Type;
    float3 Location;

    int Range;
    float3 Color;
}

// location brush�� location
float3 BrushColor(float3 location)
{
    // 0�̸� ������
    if(Type == 0)
        return float3(0, 0, 0);

    // �����ϸ� else if �Ⱦ��°� ���ٰ� �Ͻ� ���߿� �������ֽǲ�
    if (Type == 1 || Type == 4)
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
    float4 Position : SV_POSITION; // SV�� ������ ����� ������ ���Ѵ�� ���� ��
    float2 Uv : UV0;
    float3 Normal : NORMAL0;

    float4 ColorMap : COLOR0;

    float3 BrushColor : COLOR1;
};

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    // ��� ���� ��ȯ�� ���Ŀ� position�� ���� �ؾߵǱ� ��
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);

    output.ColorMap = input.Color;

    // oPosition �� 
    output.BrushColor = BrushColor(input.Position.xyz);

    output.Uv = input.Uv;

    return output;
}

Texture2D ColorMap : register(t10);
SamplerState ColorSampler : register(s10);

Texture2D ColorMap2 : register(t11);
SamplerState ColorSampler2 : register(s11);

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    float4 colorMap = ColorMap.Sample(ColorSampler, input.Uv);
    float4 colorMap2 = ColorMap2.Sample(ColorSampler2, input.Uv);
    float4 alphaMap = input.ColorMap;

    float4 alpha = float4(alphaMap.r, alphaMap.r, alphaMap.r, alphaMap.r);
    float4 temp = lerp(colorMap, colorMap2, alpha);

    DiffuseLighting(color, temp, input.Normal);

    color = color + float4(input.BrushColor, 0);

    return color;
    // ����� �ϱ� ���ؼ� normal�� ��ȯ���� ��������
    //return float4((input.Normal * 0.5f) + 0.5f, 1);
}