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
    int Width;
    int Height;
    int Count;
}

SamplerState Sampler : register(s10); // sampler라서 버퍼 아니라 s0
// 기본꺼 써도 됨 안넣어져도 기본거 있음
Texture2D Map : register(t10); // texture라 t0

static const int BlurCount = 20;
float4 PS(PixelInput input) : SV_TARGET
{
	// 보기 편해서 이렇게 짜는거 이것도 for문은 돔
    float2 arr[9] =
    {
        float2(-1, -1), float2(0, -1), float2(1, -1),
        float2(-1, 0),	float2(0,  0), float2(1,  0),
        float2(-1, 1),  float2(0,  1), float2(1,  1),
    };


    float3 color = 0;
    for (int blur = 0; blur < Count; blur++)
    {
        for (int i = 0; i < 9; i++)
        {
            float x = arr[i].x * (float) blur / (float) Width;
            float y = arr[i].y * (float) blur / (float) Height;

            float2 uv = input.Uv + float2(x, y);
            color += Map.Sample(Sampler, uv).rgb;
        }
    }

    color /= Count * 9;

    return float4(color, 1);

}

float4 PS3(PixelInput input) : SV_TARGET
{
    float x = input.Uv.x;
    float y = input.Uv.y;
   
    float4 color = Map.Sample(Sampler, float2(x, y));

    float2 arr[BlurCount];
	
    float2 temp;
    for (int i = 0; i < Count; i++)
    {
        arr[i] = float2(0, 0);
        arr[i].x = (i + 1) / (float) Width;
        arr[i].y = (i + 1) / (float) Height;

        temp = float2(x - arr[i].x, y);
        color += Map.Sample(Sampler, temp);

        temp = float2(x + arr[i].x, y);
        color += Map.Sample(Sampler, temp);

        temp = float2(x, y - arr[i].y);
        color += Map.Sample(Sampler, temp);

        temp = float2(x, y + arr[i].y);
        color += Map.Sample(Sampler, temp);
    }

    color /= (float) Count * 4.0f + 1.0f;
    color.a = 1;

    return color;
}

float4 PS2(PixelInput input) : SV_TARGET
{
    float x = input.Uv.x;
    float y = input.Uv.y;

    float4 color = Map.Sample(Sampler, float2(x, y));

    float ratioX = 1 / Width;
    float ratioY = 1 / Height;

    float2 left = float2(x - ratioX, y);
    float2 right = float2(x + ratioX, y);
    float2 upper = float2(x, y - ratioY);
    float2 lower = float2(x, y + ratioY);

    color += Map.Sample(Sampler, left);
    color += Map.Sample(Sampler, right);
    color += Map.Sample(Sampler, upper);
    color += Map.Sample(Sampler, lower);

    color.rgb /= 5;
    color.a = 1;

    return color;
}