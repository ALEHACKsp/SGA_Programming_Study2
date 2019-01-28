#include "000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

struct Test
{
    float3 Position;
};
StructuredBuffer<Test> Input;

struct VertexInput
{
    float4 Position : Position0;
    uint Id : SV_VertexID; // �갡 ��ǻ�ͽ��̴� ó�� dispatch ID�� �ɲ�
	// Draw index�� �׷��� VertexID ���� ��
};

struct VertexOutput
{
    float4 Position : SV_Position0; // 0�ۿ� ��� �Ұ���
    uint Id : VertexID0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Position.xyz = input.Position.xyz + Input[input.Id].Position;
    output.Position.w = 1.0f;

    output.Id = input.Id;

    return output;
}

//-----------------------------------------------------------------------------
// Pisxel Shader
//-----------------------------------------------------------------------------

float4 PS(VertexOutput input) : SV_TARGET
{
    float4 color = 0;
    if (input.Id % 2 == 0)
        return float4(1, 0, 0, 1);
	else
        return float4(0, 0, 1, 1);

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
