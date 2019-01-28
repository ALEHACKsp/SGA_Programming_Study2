#include "000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float3 wPosition : POSITION1;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
};

VertexOutput VS(VertexTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);
    output.Uv = input.Uv;

    return output;
}

//-----------------------------------------------------------------------------
// States
//-----------------------------------------------------------------------------

SamplerState Sampler
{
    AddressU = Wrap;
    AddressV = Wrap;
};

//-----------------------------------------------------------------------------
// PisxelShader
//-----------------------------------------------------------------------------

float4 PS(VertexOutput input/*, uniform bool bTexture*/) : SV_TARGET
{
    float4 color2 = DiffuseMap.Sample(Sampler, input.Uv);
    color2 *= dot(-LightDirection, normalize(input.Normal));

    return color2;

	//// 이렇게 하는거 아닌거 같음
 //   //Ambient = DiffuseMap.Sample(Sampler, input.Uv);
 //   Ambient = float4(0, 0, 0, 1);
 //   Diffuse = DiffuseMap.Sample(Sampler, input.Uv);
 //   //Diffuse = NormalMap.Sample(Sampler, input.Uv);
 //   Specular = SpecularMap.Sample(Sampler, input.Uv);

 //   LightAmbient = float4(0, 0, 0, 1);
 //   LightDiffuse = float4(1, 1, 1, 1);
 //   LightSpecular = float4(1, 1, 1, 1);

    float3 normal = normalize(input.Normal);
    float3 toEye = normalize(ViewPosition - input.wPosition);

    float3 ambient = float3(0, 0, 0);
    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);

    float4 texColor = DiffuseMap.Sample(Sampler, input.Uv);

    Material m = { Ambient, Diffuse, Specular, Shininess };
    DirectionalLight l = { LightAmbient, LightDiffuse, LightSpecular, LightDirection };

    float4 A, D, S;
    ComputeDirectionalLight(m, l, normal, toEye, A, D, S);
    ambient += A;
    diffuse += D;
    specular += S;

	// Context에서 넣어주면 됨
    [unroll]
    for (int i = 0; i < PointLightCount; i++)
    {
        ComputePointLight(m, PointLights[i], input.wPosition, normal, toEye, A, D, S);

        ambient += A;
        diffuse += D;
        specular += S;
    }
    
    [unroll]
    for (i = 0; i < SpotLightCount; i++)
    {
        ComputeSpotLight(m, SpotLights[i], input.wPosition, normal, toEye, A, D, S);

        ambient += A;
        diffuse += D;
        specular += S;
    }

    float4 color = texColor * float4(ambient + diffuse, 1) + float4(specular, 1);
    color.a = Diffuse.a;

    return color;
}

//-----------------------------------------------------------------------------
// Techinques
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    //pass P1
    //{
    //    SetVertexShader(CompileShader(vs_5_0, VS()));
    //    SetPixelShader(CompileShader(ps_5_0, PS(true)));
    //}
}
