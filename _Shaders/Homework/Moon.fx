#include "../000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader 
//-----------------------------------------------------------------------------

struct PixelInput
{
    float4 Position : SV_POSITION0;
    float2 Uv : UV0;
};

PixelInput VS(VertexTexture input)
{
    PixelInput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    return output;
}

//-----------------------------------------------------------------------------
// Pisxel Shader
//-----------------------------------------------------------------------------

cbuffer PS_Moon
{
    float Alpha;
}

Texture2D MoonMap;
Texture2D GlowMap;
SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PS_Moon(PixelInput input) : SV_TARGET
{
    float4 color = MoonMap.Sample(Sampler, input.Uv);
    color.a *= Alpha;

    return color;
}

float4 PS_Glow(PixelInput input) : SV_TARGET
{
    float4 color = GlowMap.Sample(Sampler, input.Uv);
    color.a *= Alpha;

    return color;
}

//-----------------------------------------------------------------------------
// States
//-----------------------------------------------------------------------------

BlendState Blend
{
    BlendEnable[0] = true;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
	
    DestBlendAlpha = ZERO;
    SrcBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
};

//-----------------------------------------------------------------------------
// Techinques
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetBlendState(Blend, float4(0, 0, 0, 0), 0xFFFFFFFF);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Moon()));
    }

    pass P1
    {
        SetBlendState(Blend, float4(0, 0, 0, 0), 0xFFFFFFFF);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Glow()));
    }
}