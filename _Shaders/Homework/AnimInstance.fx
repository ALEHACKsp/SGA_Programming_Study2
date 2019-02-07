#include "../000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

Texture2DArray Transforms;

struct FrameDesc
{
    uint Curr;
    uint Next;
    float Time;
    float FrameTime;
};

FrameDesc Frames[64];

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

int ClipId;

struct VertexOutput
{
    float4 Position : SV_Position0; // 0밖에 사용 불가함
    float2 Uv : Uv0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;

    float boneIndices[4] =
    {
        input.BlendIndices.x,
        input.BlendIndices.y,
        input.BlendIndices.z,
        input.BlendIndices.w,
    };

    float boneWeights[4] =
    {
        input.BlendWeights.x,
        input.BlendWeights.y,
        input.BlendWeights.z,
        input.BlendWeights.w,
    };

    matrix curr = 0;
    matrix next = 0;
    matrix transform = 0;
    matrix anim = 0;

    [unroll]
    for (int i = 0; i < 4; i++)
    {
        c0 = Transforms.Load(int4(boneIndices[i] * 4 + 0, Frames[input.InstID].Curr, ClipId, 0));
        c1 = Transforms.Load(int4(boneIndices[i] * 4 + 1, Frames[input.InstID].Curr, ClipId, 0));
        c2 = Transforms.Load(int4(boneIndices[i] * 4 + 2, Frames[input.InstID].Curr, ClipId, 0));
        c3 = Transforms.Load(int4(boneIndices[i] * 4 + 3, Frames[input.InstID].Curr, ClipId, 0));
        matrix curr = matrix(c0, c1, c2, c3);

        n0 = Transforms.Load(int4(boneIndices[i] * 4 + 0, Frames[input.InstID].Next, ClipId, 0));
        n1 = Transforms.Load(int4(boneIndices[i] * 4 + 1, Frames[input.InstID].Next, ClipId, 0));
        n2 = Transforms.Load(int4(boneIndices[i] * 4 + 2, Frames[input.InstID].Next, ClipId, 0));
        n3 = Transforms.Load(int4(boneIndices[i] * 4 + 3, Frames[input.InstID].Next, ClipId, 0));
        next = matrix(n0, n1, n2, n3);

        anim = lerp(curr, next, (matrix) Frames[input.InstID].Time);
        
        transform += mul(boneWeights[i], anim);
    }
    
    World = mul(transform, input.Transform);

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;
    //output.Uv = float2(Frames[input.InstID].Curr, Frames[input.InstID].Next);

    return output;
}


//-----------------------------------------------------------------------------
// Pisxel Shader
//-----------------------------------------------------------------------------

SamplerState Sampler;
float4 PS(VertexOutput input) : SV_TARGET
{
    return DiffuseMap.Sample(Sampler, input.Uv);
    //return float4(input.Uv.x, input.Uv.y, 0.0f, 1);
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
