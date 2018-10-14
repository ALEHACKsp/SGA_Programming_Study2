#include "000_Header.hlsl"

cbuffer VS_Project : register(b10)
{
    matrix V;
    matrix P;
}

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
    float4 pPosition : POSITION0; // projection 된 position
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    matrix world = BoneWorld();
    output.Position = mul(input.Position, world);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = WorldNormal(input.Normal, world);
    output.Uv = input.Uv;

    output.pPosition = mul(input.Position, world);
    output.pPosition = mul(output.pPosition, V);
    output.pPosition = mul(output.pPosition, P);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;
    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);
    DiffuseLighting(color, diffuse, input.Normal);

	// 그림자도 같은 방법 씀

    float2 projUv = 0;
	// -1 ~ 1로 -> 0 ~ 1까지로 변환할 때
	// 원본 비율이 w position이 viewprojection 변환된 결과라서
    projUv.x = input.pPosition.x / input.pPosition.w * 0.5f + 0.5f;
    projUv.y = -input.pPosition.y / input.pPosition.w * 0.5f + 0.5f;

	// 화면에 들어오는 값인지 확인
	// saturate 0~1로 떨어짐
    if (saturate(projUv.x) == projUv.x && saturate(projUv.y) == projUv.y)
	// 원처럼 나오게함
    //if ((projUv.x - 0.5f) * (projUv.x - 0.5f) + (projUv.y - 0.5f) * (projUv.y - 0.5f) < 1)
    {
		// 임의의 텍스처 노멀맵에 집어넣을꺼
        float4 projMap = NormalMap.Sample(NormalSampler, projUv);

        color = projMap;
    }

    return color;
}