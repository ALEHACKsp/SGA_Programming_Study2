#include "../000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader 
//-----------------------------------------------------------------------------

struct PixelInput
{
    float4 Position : SV_POSITION;
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

cbuffer PS_Clouds
{
    float CloudTime;
    float NumTiles = 16.0f;
    float CloudCover = -0.1f;
    float CloudSharpness = 0.5f;

    float3 SunColor;
}

Texture2D CloudsMap;
SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Wrap;
    AddressV = Wrap;
};

static const float ONE = 0.00390625f;
static const float ONEHALF = 0.001953125f;
// The numbers above are 1/256 and 0.5/256, change accordingly
// if you change the code to use another texture size.

float fade(float t)
{
  //return t*t*(3.0-2.0*t);
  return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float noise(float2 P)
{
    float2 Pi = ONE * floor(P) + ONEHALF;
    float2 Pf = frac(P);

    float2 grad00 = CloudsMap.Sample(Sampler, Pi).rg * 4.0 - 1.0;
    float n00 = dot(grad00, Pf);

    float2 grad10 = CloudsMap.Sample(Sampler, Pi + float2(ONE, 0.0)).rg * 4.0 - 1.0;
    float n10 = dot(grad10, Pf - float2(1.0, 0.0));

    float2 grad01 = CloudsMap.Sample(Sampler, Pi + float2(0.0, ONE)).rg * 4.0 - 1.0;
    float n01 = dot(grad01, Pf - float2(0.0, 1.0));

    float2 grad11 = CloudsMap.Sample(Sampler, Pi + float2(ONE, ONE)).rg * 4.0 - 1.0;
    float n11 = dot(grad11, Pf - float2(1.0, 1.0));

    float2 n_x = lerp(float2(n00, n01), float2(n10, n11), fade(Pf.x));

    float n_xy = lerp(n_x.x, n_x.y, fade(Pf.y));

    return n_xy;
}

float4 PS(PixelInput input) : SV_TARGET
{
    input.Uv = input.Uv * NumTiles;

    float n = noise(input.Uv + CloudTime);
    float n2 = noise(input.Uv * 2 + CloudTime);
    float n3 = noise(input.Uv * 4 + CloudTime);
    float n4 = noise(input.Uv * 8 + CloudTime);
	
    float nFinal = n + (n2 / 2) + (n3 / 4) + (n4 / 8);
	
    float c = CloudCover - nFinal;
    if (c < 0) 
        c = 0;
 
    float CloudDensity = 1.0 - pow(CloudSharpness, c);
    
    float4 color = CloudDensity;
    color *= float4(SunColor, 1);

	// test
    //color = CloudsMap.Sample(Sampler, input.Uv);
    
    return color;
}

//-----------------------------------------------------------------------------
// States
//-----------------------------------------------------------------------------

BlendState Blend
{
    BlendEnable[0] = true;
    //SrcBlend = ONE; // 왜 주석해야되는지 모르겠음
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
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

}