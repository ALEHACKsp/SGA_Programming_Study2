#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION; // SV만 정해진 예약어 나머진 편한대로 쓰면 됨
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

    // 사실 월드 변환된 이후에 position에 따라 해야되긴 함
    output.Position = mul(input.Position, world);
    //output.Position = mul(output.Position, BoneScale);

    output.ViewDir = WorldViewDirection(output.Position);

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

	output.Uv = input.Uv;

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
    // 디버깅 하기 위해서 normal을 반환형인 색상값으로
    //return float4((input.Normal * 0.5f) + 0.5f, 1);
}