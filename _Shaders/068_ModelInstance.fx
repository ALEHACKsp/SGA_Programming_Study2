#include "000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

Texture2D Transforms;

struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
    float4 BlendIndices : BLENDINDICES0;
    float4 BlendWeights : BLENDWEIGHTS0;

    matrix Transform : Instance0;
    uint InstID : SV_InstanceID;
};


struct VertexOutput
{
    float4 Position : SV_Position0; // 0밖에 사용 불가함
    float2 Uv : Uv0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    float4 m0 = Transforms.Load(int3(BoneIndex * 4 + 0, input.InstID, 0));
    float4 m1 = Transforms.Load(int3(BoneIndex * 4 + 1, input.InstID, 0));
    float4 m2 = Transforms.Load(int3(BoneIndex * 4 + 2, input.InstID, 0));
    float4 m3 = Transforms.Load(int3(BoneIndex * 4 + 3, input.InstID, 0));

    matrix transform = matrix(m0, m1, m2, m3);
    World = mul(transform, input.Transform);
    //World = transform;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    return output;
}


//-----------------------------------------------------------------------------
// Pisxel Shader
//-----------------------------------------------------------------------------

//RasterizerState Cull
//{
//    //FillMode = Wireframe;
//    FrontCounterClockwise = true;
//};

SamplerState Sampler;
float4 PS(VertexOutput input) : SV_TARGET
{
    return DiffuseMap.Sample(Sampler, input.Uv);
}


//-----------------------------------------------------------------------------
// Techinques
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        //SetRasterizerState(Cull);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}
