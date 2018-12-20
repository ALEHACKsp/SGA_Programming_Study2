#include "000_Header.fx"

int TsAmount;
int TsAmountInside;

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
    float Edges[3] : SV_TessFactor; // 삼각형의 변
    float Inside : SV_InsideTessFactor; // 삼각형 내부 무게 중심
};

ConstantOutput HS_Constant(InputPatch<VertexOutput, 3> input, uint patchID : SV_PrimitiveId)
{
    ConstantOutput output;

    output.Edges[0] = TsAmount;
    output.Edges[1] = TsAmount;
    output.Edges[2] = TsAmount;

    output.Inside = TsAmountInside;

    return output;
}

struct HullOutput
{
    float4 Position : Position0;
};

[domain("tri")] // 삼각형 용도로 쓰겠다는거
// line, line-adj, tri, tri-adj 등등 있음 HS 라고 치면 ms 메뉴얼 나옴 // adj 는 인접 점 4개 들어가면 띄엄띄엄 들어감 점선처럼
[partitioning("integer")] // 잘라내는 기준이 int형만 들어갈 수 있음 float이면 무시됨 float 쓰면 정밀해지는데 속도는 느려짐
// 대규모 지형에서 이동시 팝핑현상 나타는 이유가 이 때문임
[outputtopology("triangle_cw")] // 정점을 어떤 방향으로 감을껀지 항상 시계방향으로 써왔음
[outputcontrolpoints(3)] // 컨트롤 포인터 몇개 단위로 보낼지
[patchconstantfunc("HS_Constant")] // 사용 할 상수 hull shader 이름이 뭔지

//[maxtessfactor] // 내가 써줄 최대 tess factor 최대 32 * 2 까지 가능 최적화 맞추기 위해 64개로 맞춘다고 함

HullOutput HS(InputPatch<VertexOutput, 3> input, uint pointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
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

[domain("tri")]
DomainOutput DS(ConstantOutput input, float3 uvw : SV_DomainLocation, const OutputPatch<HullOutput, 3> patch)
{
    DomainOutput output;

    float3 position = uvw.x * patch[0].Position + uvw.y * patch[1].Position + uvw.z * patch[2].Position;

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
