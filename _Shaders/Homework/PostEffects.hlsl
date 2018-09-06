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

    float4 Gamma;
}

SamplerState Sampler : register(s10); // sampler라서 버퍼 아니라 s0
// 기본꺼 써도 됨 안넣어져도 기본거 있음
Texture2D Map : register(t10); // texture라 t0

static const int BlurCount = 20;
float4 PS(PixelInput input) : SV_TARGET
{
    float x = input.Uv.x;
    float y = input.Uv.y;
   
    float4 color = Map.Sample(Sampler, input.Uv);

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
    else if (Select == 3)
    {
		// Blur
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
    }
	else if (Select == 4)
    {
		// Mosaic
        color = 0;
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
    }
    else if (Select == 5)
    {
		// Gamma
        color.rgb = pow(color.rgb, 1.0f / Gamma.rgb);
    }
    else if (Select == 6)
    {
		// Bit Planner Slicing
        int r = (int) (color.r * 255);
        int g = (int) (color.g * 255);
        int b = (int) (color.b * 255);

        int slice = pow(2, Count);

        r = r / slice;
        g = g / slice;
        b = b / slice;

        r = r * slice;
        g = g * slice;
        b = b * slice;

        color.rgb = float3(r / 255.0f, g / 255.0f, b / 255.0f);

    }
    else if (Select == 7)
    {
		// Embossing
        color.rgb = float3(0.5f, 0.5f, 0.5f);

        float2 pixel = float2(1.0f / Width, 1.0f / Height);

        color = color - Map.Sample(Sampler, input.Uv - pixel) * Count;
        color = color + Map.Sample(Sampler, input.Uv + pixel) * Count;

        float temp = (color.r + color.g + color.b) / 3.0f;
        color.rgb = float3(temp, temp, temp);
    }
    else if (Select == 8)
    {
		// Sharpening 2
        float dx = 1.0f / Width;
        float dy = 1.0f / Height;
        color = 0;

		// 방법 1
        color = color + -1 * Map.Sample(Sampler, input.Uv + float2(-1.0f * dx, 0.0f * dy));
        color = color + -1 * Map.Sample(Sampler, input.Uv + float2(0.0f * dx, -1.0f * dy));
        color = color +  5 * Map.Sample(Sampler, input.Uv + float2(0.0f * dx, 0.0f * dy));
        color = color + -1 * Map.Sample(Sampler, input.Uv + float2(0.0f * dx, 1.0f * dy));
        color = color + -1 * Map.Sample(Sampler, input.Uv + float2(1.0f * dx, 0.0f * dy));
    }
    else if (Select == 9)
    {
		// Sharpening 2
        float dx = 1.0f / Width;
        float dy = 1.0f / Height;
        float4 blur = color;

		// 방법 2
        blur = blur + Map.Sample(Sampler, input.Uv + float2(0.0f * dx, 1.0f * dy)) * 0.25f;
        blur = blur + Map.Sample(Sampler, input.Uv + float2(0.0f * dx, -1.0f * dy)) * 0.25f;
        blur = blur + Map.Sample(Sampler, input.Uv + float2(1.0f * dx, 0.0f * dy)) * 0.25f;
        blur = blur + Map.Sample(Sampler, input.Uv + float2(-1.0f * dx, 0.0f * dy)) * 0.25f;
        blur = blur + Map.Sample(Sampler, input.Uv + float2(-1.0f * dx, 1.0f * dy)) * 0.25f;
        blur = blur + Map.Sample(Sampler, input.Uv + float2(-1.0f * dx, -1.0f * dy)) * 0.25f;
        blur = blur + Map.Sample(Sampler, input.Uv + float2(1.0f * dx, 1.0f * dy)) * 0.25f;
        blur = blur + Map.Sample(Sampler, input.Uv + float2(1.0f * dx, -1.0f * dy)) * 0.25f;

        blur = blur / 3.0f;
        color = lerp(blur, color, Count);
    }

    color.a = 1;
    return color;
}