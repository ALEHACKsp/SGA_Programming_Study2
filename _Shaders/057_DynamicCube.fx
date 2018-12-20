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

matrix View2;
matrix Projection2;

VertexOutput VS(VertexTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);
    output.Tangent = normalize(mul(input.Tangent, (float3x3) World));
    output.Uv = input.Uv;

    return output;
}

VertexOutput VS2(VertexTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View2);
    output.Position = mul(output.Position, Projection2);

    output.Normal = mul(input.Normal, (float3x3) World);
    output.Tangent = normalize(mul(input.Tangent, (float3x3) World));
    output.Uv = input.Uv;

    return output;
}

///////////////////////////////////////////////////////////////////////////////

VertexOutput VS_Model(VertexTextureNormalTangentBlend input)
{
    VertexOutput output;

    World = BoneWorld();
    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = normalize(mul(input.Tangent, (float3x3) World));
    output.Uv = input.Uv;


    return output;
}

VertexOutput VS2_Model(VertexTextureNormalTangentBlend input)
{
    VertexOutput output;

    World = BoneWorld();
    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View2);
    output.Position = mul(output.Position, Projection2);

    output.Normal = WorldNormal(input.Normal);
    output.Tangent = normalize(mul(input.Tangent, (float3x3) World));
    output.Uv = input.Uv;


    return output;
}

//-----------------------------------------------------------------------------
// States
//-----------------------------------------------------------------------------

SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

TextureCube CubeMap;

//-----------------------------------------------------------------------------
// Pisxel Shader
//-----------------------------------------------------------------------------

float4 PS(VertexOutput input) : SV_TARGET
{
    float3 normal = input.Normal;

    float3 incident = input.wPosition - ViewPosition;
    //float3 reflection = 2.0f * dot(-incident, input.Normal) * input.Normal + incident;
    float3 reflection = reflect(incident, normal);

	// นÝป็
    float3 color = CubeMap.Sample(Sampler, reflection);

    return float4(color, 1);
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

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Model()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

technique11 T1
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS2()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS2_Model()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}