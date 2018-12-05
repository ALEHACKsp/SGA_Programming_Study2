#include "000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

VertexOutput VS(VertexTexture input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

	// 이렇게하면 비율로 들어감
	// NDC 공간 - dx9에서 FVF_RHWXYZ 쓴거랑 같음
    //output.Position = input.Position;
	// ouput.Position.x / output.Position.w 
	// ouput.Position.y / output.Position.w // 나누면 NDC 공간 됨 -1 ~ 1,

    output.Uv = input.Uv;

    return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------

SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
};

Texture2D Map;

float4 PS(VertexOutput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.Uv); // 월요일날 설명해주실꺼

    return color;
}

//-----------------------------------------------------------------------------
// States
//-----------------------------------------------------------------------------

DepthStencilState Depth
{
    DepthEnable = false;
};

//-----------------------------------------------------------------------------
// Techinques
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));

        SetDepthStencilState(Depth, 0);
    }
}