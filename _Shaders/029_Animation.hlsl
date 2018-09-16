#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION; // SV�� ������ ����� ������ ���Ѵ�� ���� ��
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
    float3 ViewDir : VIEWDIR0;
};

PixelInput VS(VertexTextureNormalTangentBlend input)
{
     PixelInput output;

    matrix world = SkinWorld(input.BlendIndices, input.BlendWeights);
    //matrix world = BoneWorld();

    // ��� ���� ��ȯ�� ���Ŀ� position�� ���� �ؾߵǱ� ��
    output.Position = mul(input.Position, world);
    //output.Position = mul(output.Position, BoneScale);

    output.ViewDir = WorldViewDirection(output.Position);

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

	output.Uv = input.Uv;

    output.Normal = mul(input.Normal, (float3x3) world);
    output.Normal = normalize(output.Normal);

    output.Normal = WorldNormal(input.Normal, world);
    output.Tangent = WorldTangent(input.Tangent, world);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    //DiffuseLighting(color, diffuse, input.Normal);
    if (length(diffuse) > 0)
        DiffuseLighting(color, diffuse, input.Normal);
    else
        DiffuseLighting(color, input.Normal);

    //float4 normal = NormalMap.Sample(NormalSampler, input.Uv);
    //NormalMapping(color, normal, input.Normal, input.Tangent);

    //float4 specular = SpecularMap.Sample(SpecularSampler, input.Uv);
    //SpecularLighting(color, specular, input.Normal, input.ViewDir);

    return color;
    // ����� �ϱ� ���ؼ� normal�� ��ȯ���� ��������
    //return float4((input.Normal * 0.5f) + 0.5f, 1);
}