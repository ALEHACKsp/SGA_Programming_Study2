#include "000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float4 oPosition : POSITION1;
};

matrix View2;
matrix Projection2;

VertexOutput VS(VertexTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.oPosition = input.Position;

    return output;
}

VertexOutput VS2(VertexTextureNormalTangent input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View2);
    output.Position = mul(output.Position, Projection2);

    output.oPosition = input.Position;

    return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------

SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

TextureCube CubeMap;

float4 PS(VertexOutput input) : SV_TARGET
{
    float3 normal = normalize(input.oPosition.xyz);

    return CubeMap.Sample(Sampler, normal);
}

//-----------------------------------------------------------------------------
// States
//-----------------------------------------------------------------------------

DepthStencilState Depth
{
    //DepthEnable = false;
    DepthFunc = LESS_EQUAL;
};

RasterizerState Cull
{
    FrontCounterClockwise = true;
};

//-----------------------------------------------------------------------------
// Techinque
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));

        SetDepthStencilState(Depth, 0);
        SetRasterizerState(Cull);
    }
}

technique11 T1
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS2()));
        SetPixelShader(CompileShader(ps_5_0, PS()));

        SetDepthStencilState(Depth, 0);
        SetRasterizerState(Cull);
    }
}