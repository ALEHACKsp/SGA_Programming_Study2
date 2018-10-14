#include "000_Header.hlsl"

cbuffer VS_Project : register(b10)
{
    matrix V;
    matrix P;
}

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
    float4 pPosition : POSITION0; // projection �� position
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    matrix world = BoneWorld();
    output.Position = mul(input.Position, world);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = WorldNormal(input.Normal, world);
    output.Uv = input.Uv;

    output.pPosition = mul(input.Position, world);
    output.pPosition = mul(output.pPosition, V);
    output.pPosition = mul(output.pPosition, P);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(color, diffuse, input.Normal);

	// �׸��ڵ� ���� ��� ��

    float2 projUv = 0;
	// -1 ~ 1�� -> 0 ~ 1������ ��ȯ�� ��
	// ���� ������ w position�� viewprojection ��ȯ�� �����
    projUv.x = input.pPosition.x / input.pPosition.w * 0.5f + 0.5f;
    projUv.y = -input.pPosition.y / input.pPosition.w * 0.5f + 0.5f;

	// ȭ�鿡 ������ ������ Ȯ��
	// saturate 0~1�� ������
    if (saturate(projUv.x) == projUv.x && saturate(projUv.y) == projUv.y)
	// ��ó�� ��������
    //if ((projUv.x - 0.5f) * (projUv.x - 0.5f) + (projUv.y - 0.5f) * (projUv.y - 0.5f) < 1)
    {
		// ������ �ؽ�ó ��ָʿ� ���������
        float4 projMap = NormalMap.Sample(NormalSampler, projUv);

        color = projMap;
    }

    return color;
}