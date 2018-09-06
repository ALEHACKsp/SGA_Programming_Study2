#include "../000_Header.hlsl"

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

static const int MosaicCount = 20;
float4 PS(PixelInput input) : SV_TARGET
{
    float x = input.Uv.x;
    float y = input.Uv.y;
   
    float4 color = 0;

    float2 offset = fmod(float2(x * Width, y * Height), Count);

    for (float i = 0.0f; i < Count; i++)
    {
        for (float j = 0.0f; j < Count; j++)
        {
            color += Map.Sample(Sampler,
			input.Uv - float2((offset.x + i) / Width, (offset.y + j) / Height));
        }
    }

    color = color / pow(Count, 2.0f);

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