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

	// �̷����ϸ� ������ ��
	// NDC ���� - dx9���� FVF_RHWXYZ ���Ŷ� ����
    //output.Position = input.Position;
	// ouput.Position.x / output.Position.w 
	// ouput.Position.y / output.Position.w // ������ NDC ���� �� -1 ~ 1,

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
    float4 color = Map.Sample(Sampler, input.Uv); // �����ϳ� �������ֽǲ�

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