#include "000_Header.hlsl"

// uv는 픽셀 쉐이더에서 처리
// 샘플링 얘기하면서 설명해주실꺼

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

SamplerState Sampler : register(s10); // sampler라서 버퍼 아니라 s0
// 기본꺼 써도 됨 안넣어져도 기본거 있음
Texture2D Map : register(t10); // texture라 t0

PixelInput VS(VertexTexture input)
{
    PixelInput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

	// 이렇게하면 비율로 들어감
	// NDC 공간 - dx9에서 FVF_RHWXYZ 쓴거랑 같음
    //output.Position = input.Position;
	// ouput.Position.x / output.Position.w 
	// ouput.Position.y / output.Position.w // 나누면 NDC 공간 됨 -1 ~ 1,

    output.Uv = input.Uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.Uv); // 월요일날 설명해주실꺼

    return color;
}