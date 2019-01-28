#include "000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

Texture2D WorldData;

struct VertexInput
{
    float4 Position : Position0;
    uint Id : SV_VertexID; // 얘가 컴퓨터쉐이더 처럼 dispatch ID가 될꺼

    float4 Color : Instance0;
    uint InstID : SV_InstanceID;
};

struct VertexOutput
{
    float4 Position : SV_Position0; // 0밖에 사용 불가함
    float4 Color : Color0;
    uint Id : VertexID0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    //float3 position = WorldData.Load(int3(input.InstID * 4 + 3, 0, 0)).xyz;

    float3 position = 0;
    position.x = WorldData.Load(int3(0 + 3, input.InstID, 0)).x;
    position.y = WorldData.Load(int3(4 + 3, input.InstID, 0)).y;
    position.z = WorldData.Load(int3(8 + 3, input.InstID, 0)).z;

    output.Position.xyz = input.Position.xyz + position;
    output.Position.w = 1.0f;

    output.Color = input.Color;
    output.Id = input.Id;

    return output;
}

VertexOutput VS2(VertexInput input)
{
    VertexOutput output;

    float3 position = WorldData.Load(int3(input.InstID * 4 + 3, 0, 0)).xyz;

    output.Position.xyz = input.Position.xyz + position;
    output.Position.w = 1.0f;

    output.Color = input.Color;
    output.Id = input.Id;

    return output;
}

//-----------------------------------------------------------------------------
// Pisxel Shader
//-----------------------------------------------------------------------------

float4 PS(VertexOutput input) : SV_TARGET
{
    return input.Color;
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

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS2()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}
