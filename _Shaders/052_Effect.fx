#include "000_Header.fx"

//-----------------------------------------------------------------------------
// States
//-----------------------------------------------------------------------------
RasterizerState CullModeOn // �ƹ��͵� �������ϸ� �⺻����
{
    FillMode = Wireframe;
};

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : SV_POSITION;
};

VertexOutput VS(Vertex input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------

float4 PS(VertexOutput input) : SV_TARGET
{
    return float4(1, 0, 0, 1);
}

float4 PS2(VertexOutput input) : SV_TARGET
{
    return float4(0, 0, 1, 1);
}

//-----------------------------------------------------------------------------
// Technique
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        //SetVertexShader(NULL); // ���� �Ⱦ��� �̷��� �����
        SetPixelShader(CompileShader(ps_5_0, PS()));

        SetRasterizerState(CullModeOn);
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS2()));
    }
}