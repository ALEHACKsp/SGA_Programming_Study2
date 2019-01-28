#include "000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : SV_Position0;
};

VertexOutput VS(Vertex input)
{
    VertexOutput output;
    output.Position = input.Position; // NDC 공간으로 들어감

    return output;
}


//-----------------------------------------------------------------------------
// Pisxel Shader
//-----------------------------------------------------------------------------

float4 PS(VertexOutput output) : SV_TARGET
{
    return float4(1, 0, 0, 1);
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

}
