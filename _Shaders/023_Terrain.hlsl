#include "000_Header.hlsl"

cbuffer PS_Line : register(b10)
{
    int LineType;
    float3 LineColor; 

    int Spacing; // ��ĭ�� ����
    float Thickness; // ���� �β�
}


float3 Line(float3 oPosition)
{
    [branch] // branch ���� else if �� �� ����
    // �̰Ÿ��� flatten�� ����
    // ���̴������� if else�� �Ѵ� ���� �� ���ǿ� �´°Ÿ� ��
    // flatten �� �� ���� �� �� ���� ���� else if ����
    // branch�� �츮�� �ƴ� c������� ��
    if (LineType == 1)
    {
        float2 grid = float2(0, 0);
        grid.x = frac(oPosition.x / (float) Spacing); // spacing ���� ���� �� 0~1 ���̷� �������
        grid.y = frac(oPosition.z / (float) Spacing); // spacing ���� ���� �� 0~1 ���̷� �������

        [flatten]
        if(grid.x < Thickness || grid.y < Thickness)
            return LineColor;
    }
    else if (LineType == 2)
    {
        float2 grid = oPosition.xz / (float) Spacing;

        // 0.5 ���Ŵ� �������� ���߷��� �ϴ°� (0.5f ��� ���� ��)
        float2 range = abs(frac(grid - 0.5f) - 0.5f);
        float2 speed = fwidth(grid); // ���̺�, ���� ����
        // = abs(ddx(x) + ddy(y))

        // ���� �α��� ������ַ��� �ϴ°�
        float2 pixel = range / speed;
        // �̺κ��� �����غ���
        float weights = saturate(min(pixel.x, pixel.y) - Thickness);

        // �����̶� ���� ������ �ִ� ���̶� ����
        return lerp(LineColor, float3(0, 0, 0), weights);
    }

    return float3(0, 0, 0);
}

struct PixelInput
{
    float4 Position : SV_POSITION; // SV�� ������ ����� ������ ���Ѵ�� ���� ��
    float3 oPosition : POSITION0;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.oPosition = input.Position;

    // ��� ���� ��ȯ�� ���Ŀ� position�� ���� �ؾߵǱ� ��
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);

    output.Uv = input.Uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);

    DiffuseLighting(color, diffuse, input.Normal);

    // Line Drawing
    color = color + float4(Line(input.oPosition), 0);

    return color;
    // ����� �ϱ� ���ؼ� normal�� ��ȯ���� ��������
    //return float4((input.Normal * 0.5f) + 0.5f, 1);
}