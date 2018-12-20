#include "000_Header.fx"

float Ratio = 20;

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : Position0; // �ȼ��� �����°� �ƴϾ SV ����
    float2 Uv : Uv0;
    float3 Normal : Normal0;
};

VertexOutput VS(VertexTextureNormal input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Normal = WorldNormal(input.Normal);
    output.Uv = input.Uv;

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

int ComputeAmount(float3 position)
{
    float dist = distance(position, ViewPosition);
    float s = saturate((dist - Ratio) / (100 - Ratio));

    return (int) lerp(6, 1, s);
}

ConstantOutput HS_Constant(InputPatch<VertexOutput, 4> input, uint patchID : SV_PrimitiveId)
{
    ConstantOutput output;

    float3 center = 0;
    center = (input[0].Position + input[1].Position) * 0.5f;
    output.Edges[0] = ComputeAmount(center);

    center = (input[1].Position + input[3].Position) * 0.5f;
    output.Edges[1] = ComputeAmount(center);

    center = (input[2].Position + input[3].Position) * 0.5f;
    output.Edges[2] = ComputeAmount(center);

    center = (input[0].Position + input[2].Position) * 0.5f;
    output.Edges[3] = ComputeAmount(center);

    center = (input[0].Position + input[1].Position + input[2].Position + input[3].Position) * 0.25f;
    output.Inside[0] = ComputeAmount(center);;
    output.Inside[1] = output.Inside[0];

    return output;
}

struct HullOutput
{
    float4 Position : Position0;
    float2 Uv : Uv0;
    float3 Normal : Normal0;
};

[domain("quad")] // �ﰢ�� �뵵�� ���ڴٴ°�
// line, line-adj, tri, tri-adj ��� ���� HS ��� ġ�� ms �޴��� ���� // adj �� ���� �� 4�� ���� ������ �� ����ó��
[partitioning("integer")] // �߶󳻴� ������ int���� �� �� ���� float�̸� ���õ� float ���� ���������µ� �ӵ��� ������
//[partitioning("fractional_odd")] // �߶󳻴� ������ int���� �� �� ���� float�̸� ���õ� float ���� ���������µ� �ӵ��� ������
// ��Ը� �������� �̵��� �������� ��Ÿ�� ������ �� ������
[outputtopology("triangle_cw")] // ������ � �������� �������� �׻� �ð�������� �����
[outputcontrolpoints(4)] // ��Ʈ�� ������ � ������ ������
[patchconstantfunc("HS_Constant")] // ��� �� ��� hull shader �̸��� ����

//[maxtessfactor] // ���� ���� �ִ� tess factor �ִ� 32 * 2 ���� ���� ����ȭ ���߱� ���� 64���� ����ٰ� ��

HullOutput HS(InputPatch<VertexOutput, 4> input, uint pointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HullOutput output;
    output.Position = input[pointID].Position;
    output.Uv = input[pointID].Uv;
    output.Normal = input[pointID].Normal;

    return output;
}

//-----------------------------------------------------------------------------
// Domain Shader
//-----------------------------------------------------------------------------

struct DomainOutput
{
    float4 Position : SV_Position0;
    float2 Uv : Uv0;
    float3 Normal : Normal0;
};

[domain("quad")]
// �簢���� �������� 2���� float2 uv�� �޾Ƶ� �� �ﰢ���� float3�� �޾ƾ���
DomainOutput DS(ConstantOutput input, float2 uv : SV_DomainLocation, const OutputPatch<HullOutput, 4> patch)
{
    DomainOutput output;

    float3 v1 = lerp(patch[0].Position, patch[1].Position, 1 - uv.y);
    float3 v2 = lerp(patch[2].Position, patch[3].Position, 1 - uv.y);
    float3 p = lerp(v1, v2, uv.x);
    
    output.Position = mul(float4(p, 1.0f), View);
    output.Position = mul(output.Position, Projection);
    
    float2 uv1 = lerp(patch[0].Uv.xy, patch[1].Uv.xy, 1 - uv.y);
    float2 uv2 = lerp(patch[2].Uv.xy, patch[3].Uv.xy, 1 - uv.y);
    output.Uv = lerp(uv1, uv2, uv.x);
    
    float3 n1 = lerp(patch[0].Normal, patch[1].Normal, 1 - uv.y);
    float3 n2 = lerp(patch[2].Normal, patch[3].Normal, 1 - uv.y);
    output.Normal = lerp(n1, n2, uv.x);

    return output;
}

//-----------------------------------------------------------------------------
// Pisxel Shader
//-----------------------------------------------------------------------------

SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
};

float4 PS(DomainOutput input) : SV_TARGET
{
    float4 diffuse = DiffuseMap.Sample(Sampler, input.Uv);
    diffuse *= dot(-LightDirection, normalize(input.Normal));

    return diffuse;
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
