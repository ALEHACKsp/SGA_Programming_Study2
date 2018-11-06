#include "000_Header2.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
    float4 wPosition : POSITION0;
    float3 cPosition : POSTIION1;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    matrix world = BoneWorld();
    output.Position = mul(input.Position, world);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = WorldNormal(input.Normal, world);

    output.Uv = input.Uv;
    output.cPosition = CameraPosition();

    return output;
}


float4 PS(PixelInput input) : SV_TARGET
{
    float3 normal = normalize(input.Normal);
    float4 color = float4(Diffuse.rgb * Diffuse.rgb, Diffuse.a); // 여기서 a 감도
 
    Material material = CreateMaterial(input.Normal, input.Uv);
    //for (int i = 0; i < LightingCount; i++)
    if (LightingCount > 0)
        color.rgb = Lighting(LightingDatas[0], input.wPosition.xyz, input.cPosition, material);
    for (int i = 1; i < LightingCount; i++)
        color.rgb += Lighting(LightingDatas[i], input.wPosition.xyz, input.cPosition, material);
    //color.rgb += Lighting(LightingDatas[1], input.wPosition.xyz, input.cPosition, material);
    //color.rgb += Lighting(LightingDatas[2], input.wPosition.xyz, input.cPosition, material);
    color.a = 1.0f;

    return color;
}