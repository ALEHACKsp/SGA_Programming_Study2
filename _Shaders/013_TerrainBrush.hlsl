#include "000_Header.hlsl"

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
    float4 Position : SV_POSITION; // SV�� ������ ����� ������ ���Ѵ�� ���� ��
    float2 Uv : UV0;
    float3 Normal : NORMAL0;

    float3 BrushColor : COLOR0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    // ��� ���� ��ȯ�� ���Ŀ� position�� ���� �ؾߵǱ� ��
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);

    // oPosition �� 
    output.BrushColor = BrushColor(input.Position.xyz);

    output.Uv = input.Uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);

    DiffuseLighting(color, diffuse, input.Normal);

    // ���� �Ϸ� �� brush ��� �Ҳ�

    color = color + float4(input.BrushColor, 0);
    
    // ���� �� ��
    // color = 1;
    //DiffuseLight(color, input.Normal);

    return color;
    // ����� �ϱ� ���ؼ� normal�� ��ȯ���� ��������
    //return float4((input.Normal * 0.5f) + 0.5f, 1);
}