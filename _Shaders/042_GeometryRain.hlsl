#include "000_Header2.hlsl"

cbuffer VS_Buffer : register(b10)
{
    float3 Velocity;
    float DrawDistance;

    float4 Color;
    float3 Origin;
    float Time;

    float3 Size;
}

struct VertexInput
{
    float4 Center : POSITION0;
    float2 Scale : SCALE0;
};

struct VertexOutput
{
    float3 Center : POSITION0;
    float2 Scale : SCALE0;

    float2 Distance : DISTANCE0;
    float4 Color : COLOR0;
    float3 Velocity : VELOCITY0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    float3 velocity = Velocity;
	// y 스케일에 따라 xz의 크기를 조정해서 보여주려고 하는거
    velocity.xz /= input.Scale.y * 0.1f;

	// 위치 이동 시키는 거
    float3 displace = Time * velocity;

    //input.Position.y = Origin.y + (Size.y + (input.Position.y + displace.y) % Size.y) % Size.y;
    //if (velocity.x >= 0)
    //    input.Position.x = Origin.x + (input.Position.x + displace.x) % Size.x;
    //else
    //    input.Position.x = Origin.x + Size.x - (input.Position.x + displace.x) % Size.x;
    //if (velocity.z >= 0)
    //    input.Position.z = Origin.z + (input.Position.z + displace.z) % Size.z;
    //else
    //    input.Position.z = Origin.z + Size.z - (input.Position.z + displace.z) % Size.z;

	// 일진씨 방법
    //float3 size = Size * 2.0f;
    //int3 signValue = 0;
    //signValue = sign(velocity);
    //input.Position.xyz = Origin + (-1.0f * signValue * Size) + (input.Position.xyz + displace) % size;

	// 동헌씨 방법
    input.Center.xyz = Origin.xyz 
		+ (Size.xyz + (input.Center.xyz + displace.xyz) % Size.xyz) % Size.xyz 
		- (Size.xyz * 0.5f);

    float alpha = cos(Time + (input.Center.x + input.Center.z));
    alpha = saturate(1.5f + alpha / DrawDistance * 2);

    output.Center = input.Center.xyz;
    output.Scale = input.Scale;

    output.Color = Color;
    output.Distance.x = DrawDistance;
    output.Distance.y = alpha;

    output.Velocity = velocity;

    return output;
}

///////////////////////////////////////////////////////////////////////////////

struct GeometryOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
    float4 Color : COLOR0;
    float Alpha : ALPHA0;
};

static const float2 TexCoord[4] = { 
	float2(0, 1), 
	float2(0, 0), 
	float2(1, 1), 
	float2(1, 0) 
};

[maxvertexcount(4)]
void GS(point VertexOutput input[1], inout TriangleStream<GeometryOutput> stream)
{
    float3 up = normalize(-input[0].Velocity);
    float3 forward = CameraPosition() - input[0].Center;
    float3 right = normalize(cross(up, forward));

    float halfWidth = 0.5f * input[0].Scale.x;
    float halfHeight = 0.5f * input[0].Scale.y;

    float4 v[4];
    v[0] = float4(input[0].Center + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(input[0].Center + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(input[0].Center - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(input[0].Center - halfWidth * right + halfHeight * up, 1.0f);

    GeometryOutput output;
	[unroll]
    for (int i = 0; i < 4; i++)
    {
        output.Position = mul(v[i], View);
        output.Position = mul(output.Position, Projection);
        output.Uv = TexCoord[i];
	
        output.Alpha = 0.2f * saturate(1 - output.Position.z / input[0].Distance.x) * input[0].Distance.y;
        output.Color = input[0].Color;

        stream.Append(output);
    }
}

///////////////////////////////////////////////////////////////////////////////

Texture2D Map : register(t10);
SamplerState Sampler : register(s10);

float4 PS(GeometryOutput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.Uv);
    color.rgb *= input.Color.xyz * (1 + input.Alpha) * 2.0f;
    color.a *= input.Alpha;

    return color;
}