#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};

PixelInput VS(VertexTexture input)
{
    PixelInput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

	// 이렇게하면 비율로 들어감
	// NDC 공간
    //output.Position = input.Position;

    output.Uv = input.Uv;

    return output;
}

///////////////////////////////////////////////////////////////////////////////

cbuffer PS_Select : register(b10)
{
    int Select;
}

SamplerState Sampler : register(s10); // sampler라서 버퍼 아니라 s0
// 기본꺼 써도 됨 안넣어져도 기본거 있음
Texture2D Map : register(t10); // texture라 t0

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.Uv); // 월요일날 설명해주실꺼
    
	[branch]
    if (Select == 0)
    {
		// 수치상 회색임 사람이 제대로 못느낀다고 함 Average
        color.rgb = (color.r + color.g + color.b) / 3.0f;
    }	
    else if (Select == 1)
    {
		// Grayscale // 과학자들이 규정해놓은 수치가 있음
        float3 monotone = float3(0.299f, 0.587f, 0.114f);
        color.rgb = dot(color.rgb, monotone);
    }
	else if (Select == 2)
    {
		// Inverse
		color.rgb = saturate(1.0f - color.rgb); 
    }

    return color;
}