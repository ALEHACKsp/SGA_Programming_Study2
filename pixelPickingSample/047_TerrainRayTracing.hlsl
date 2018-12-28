#include "000_Header2.hlsl"

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 TerrainUv : TUV;
};

cbuffer PS_Terrain : register(b10)
{
    int TerrainWidth;
    int TerrainHeight;
};

VertexOutput VS(VertexTextureNormalTangentSplatting input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.TerrainUv = float2(input.Position.x / (float) TerrainWidth, input.Position.z / (float) TerrainHeight);

    return output;
}

float4 PS(VertexOutput input) : SV_TARGET
{
    float4 color = 0;
    color = float4(input.TerrainUv, 0, 1);
    return color;
}