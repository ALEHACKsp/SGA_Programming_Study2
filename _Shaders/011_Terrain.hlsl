#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION; // SV�� ������ ����� ������ ���Ѵ�� ���� ��
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

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
    
    // ���� �� ��
    // color = 1;
    //DiffuseLight(color, input.Normal);

    return color;
    // ����� �ϱ� ���ؼ� normal�� ��ȯ���� ��������
    //return float4((input.Normal * 0.5f) + 0.5f, 1);
}