#include "000_Header.fx"

float Ratio = 20;
int Piece = 1;

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Position : Position0; // 픽셀로 내보는게 아니어서 SV 뺀거
};

VertexOutput VS(Vertex input)
{
    VertexOutput output;
	
    output.Position = input.Position; // 동차변환 DS나 GS에서 할 꺼

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

    return (int) lerp(Piece, 1, s);
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
};

[domain("quad")] // 삼각형 용도로 쓰겠다는거
// line, line-adj, tri, tri-adj 등등 있음 HS 라고 치면 ms 메뉴얼 나옴 // adj 는 인접 점 4개 들어가면 띄엄띄엄 들어감 점선처럼
[partitioning("integer")] // 잘라내는 기준이 int형만 들어갈 수 있음 float이면 무시됨 float 쓰면 정밀해지는데 속도는 느려짐
//[partitioning("fractional_odd")] // 잘라내는 기준이 int형만 들어갈 수 있음 float이면 무시됨 float 쓰면 정밀해지는데 속도는 느려짐
// 대규모 지형에서 이동시 팝핑현상 나타는 이유가 이 때문임
[outputtopology("triangle_cw")] // 정점을 어떤 방향으로 감을껀지 항상 시계방향으로 써왔음
[outputcontrolpoints(4)] // 컨트롤 포인터 몇개 단위로 보낼지
[patchconstantfunc("HS_Constant")] // 사용 할 상수 hull shader 이름이 뭔지

//[maxtessfactor] // 내가 써줄 최대 tess factor 최대 32 * 2 까지 가능 최적화 맞추기 위해 64개로 맞춘다고 함

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
// 사각형은 제어점이 2개라 float2 uv로 받아도 됨 삼각형은 float3로 받아야함
DomainOutput DS(ConstantOutput input, float2 uv : SV_DomainLocation, const OutputPatch<HullOutput, 4> patch)
{
    DomainOutput output;

    float3 v1 = lerp(patch[0].Position.xyz, patch[1].Position.xyz, 1 - uv.y); // 뒤에 s는 무게중심값
    float3 v2 = lerp(patch[2].Position.xyz, patch[3].Position.xyz, 1 - uv.y); // 뒤에 s는 무게중심값
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
