#include "000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
};

VertexOutput VS_Bone(VertexTextureNormalTangentBlend input)
{
    VertexOutput output;

    World = BoneWorld();
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);
    output.Tangent = mul(input.Tangent, (float3x3) World);

    output.Uv = input.Uv;

    return output;
}

VertexOutput VS_Animation(VertexTextureNormalTangentBlend input)
{
    VertexOutput output;

    World = SkinWorld(input.BlendIndices, input.BlendWeights);
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = WorldTangent(input.Tangent);

    output.Uv = input.Uv;

    return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------

//SamplerState Sampler; // 기본값으로 쓰고 싶으면 이렇게 쓰면돔

SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PS(VertexOutput input) : SV_TARGET
{

    float4 color = 0;

    float4 diffuse = DiffuseMap.Sample(Sampler, input.Uv);

    //float3 normal = normalize(input.Normal);
    //float NDotL = dot(-LightDirection, normal);

    //return diffuse;
    //return diffuse * NDotL;
    //return diffuse * NDotL * LightColor;
    //return saturate(diffuse + LightColor) * NDotL;

    if (length(diffuse) > 0)
        DiffuseLighting(color, diffuse, input.Normal);
    else
        DiffuseLighting(color, input.Normal);

    float4 normal = NormalMap.Sample(Sampler, input.Uv);
    if (length(normal) > 0)
        NormalMapping(color, normal, input.Normal, input.Tangent);

    float4 specular = SpecularMap.Sample(Sampler, input.Uv);
    if (length(specular) > 0)
        SpecularLighting(color, specular, input.Normal);
    else
        SpecularLighting(color, input.Normal);

	return color;
}

//-----------------------------------------------------------------------------
// Technique
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Bone()));
        //SetVertexShader(NULL); // 만약 안쓰면 이렇게 쓰면됨
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Animation()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}