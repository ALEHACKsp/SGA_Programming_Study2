#include "000_Header.fx"

int TsAmount;
int TsAmountInside;

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : Position0; // �ȼ��� �����°� �ƴϾ SV ����
};

VertexOutput VS(Vertex input)
{
    VertexOutput output;
	
    output.Position = input.Position; // ������ȯ DS�� GS���� �� ��

    return output;
}

//-----------------------------------------------------------------------------
// Hull Shader
//-----------------------------------------------------------------------------

struct ConstantOutput
{
    float Edges[4] : SV_TessFactor; 
    float Inside[2] : SV_InsideTessFactor; 
};

ConstantOutput HS_Constant(InputPatch<VertexOutput, 4> input, uint patchID : SV_PrimitiveId)
{
    ConstantOutput output;

    output.Edges[0] = TsAmount;
    output.Edges[1] = TsAmount;
    output.Edges[2] = TsAmount;
    output.Edges[3] = TsAmount;

    output.Inside[0] = TsAmountInside;
    output.Inside[1] = TsAmountInside;

    return output;
}

struct HullOutput
{
    float4 Position : Position0;
};

[domain("quad")] // �ﰢ�� �뵵�� ���ڴٴ°�
// line, line-adj, tri, tri-adj ��� ���� HS ��� ġ�� ms �޴��� ���� // adj �� ���� �� 4�� ���� ������ �� ����ó��
[partitioning("integer")] // �߶󳻴� ������ int���� �� �� ���� float�̸� ���õ� float ���� ���������µ� �ӵ��� ������
// ��Ը� �������� �̵��� �������� ��Ÿ�� ������ �� ������
[outputtopology("triangle_cw")] // ������ � �������� �������� �׻� �ð�������� �����
[outputcontrolpoints(4)] // ��Ʈ�� ������ � ������ ������
[patchconstantfunc("HS_Constant")] // ��� �� ��� hull shader �̸��� ����

//[maxtessfactor] // ���� ���� �ִ� tess factor �ִ� 32 * 2 ���� ���� ����ȭ ���߱� ���� 64���� ����ٰ� ��

HullOutput HS(InputPatch<VertexOutput, 4> input, uint pointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HullOutput output;
    output.Position = input[pointID].Position;

    return output;
}

//-----------------------------------------------------------------------------
// Domain Shader
//-----------------------------------------------------------------------------

struct DomainOutput
{
    float4 Position : SV_Position0;
};

[domain("quad")]
// �簢���� �������� 2���� float2 uv�� �޾Ƶ� �� �ﰢ���� float3�� �޾ƾ���
DomainOutput DS(ConstantOutput input, float2 uv : SV_DomainLocation, const OutputPatch<HullOutput, 4> patch)
{
    DomainOutput output;

    float3 v1 = lerp(patch[0].Position.xyz, patch[1].Position.xyz, 1 - uv.y); // �ڿ� s�� �����߽ɰ�
    float3 v2 = lerp(patch[2].Position.xyz, patch[3].Position.xyz, 1 - uv.y); // �ڿ� s�� �����߽ɰ�
    float3 position = lerp(v1, v2, uv.x);

    output.Position = mul(float4(position, 1), World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    return output;
}

//-----------------------------------------------------------------------------
// Pisxel Shader
//-----------------------------------------------------------------------------

float4 PS(DomainOutput input) : SV_TARGET
{
    return float4(0, 0, 1, 1);
}

//-----------------------------------------------------------------------------
// States
//-----------------------------------------------------------------------------

RasterizerState FillMode
{
    FillMode = Wireframe;
};

//-----------------------------------------------------------------------------
// Techinques
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetRasterizerState(FillMode);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetHullShader(CompileShader(hs_5_0, HS()));
        SetDomainShader(CompileShader(ds_5_0, DS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}
