#include "000_Header.hlsl"

cbuffer PS_Sky : register(b10)
{
    float4 Center; // 구의 중점 색깔임
    float4 Apex; // 맨 꼭대기 색깔임

    float Height;
}

struct PixelInput
{
    float4 Position : SV_POSITION; // SV만 정해진 예약어 나머진 편한대로 쓰면 됨
    float4 oPosition : POSITION0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    // 사실 월드 변환된 이후에 position에 따라 해야되긴 함
    matrix world = Bones[BoneIndex]; // bone은 이렇게 해야함
    output.Position = mul(input.Position, world);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.oPosition = input.Position;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float y = saturate(input.oPosition.y);

    return lerp(Center, Apex, y * Height);
}