#include "000_Header.hlsl"

static const float3 worldUp = float3(0.0f, 1.0f, 0.0f);
static const float timeOfDay = 12.0f;
static const float MistingDistance = 50.0f;

static const float2 imgageUV[16] =
{
    float2(0.0f, 0.00f),    float2(0.25f, 0.00f), float2(0.50f, 0.00f), float2(0.75f, 0.00f),
	float2(0.0f, 0.25f),    float2(0.25f, 0.25f), float2(0.50f, 0.25f), float2(0.75f, 0.25f),
	float2(0.0f, 0.50f),    float2(0.25f, 0.50f), float2(0.50f, 0.50f), float2(0.75f, 0.50f),
	float2(0.0f, 0.75f),    float2(0.25f, 0.75f), float2(0.50f, 0.75f), float2(0.75f, 0.75f)
};

Texture2D partTexture : register(t10);
SamplerState partTextureSampler : register(s10);

cbuffer PS_SpotLights : register(b5)
{
    float4 lightColor;
}


struct VertexTextureInstance
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    
    float4x4 instanceTransform : INSTANCE0;
    float4 instancePadding : INSTANCE4;
};

struct PixelInput
{
    float4 Position : SV_POSITION;
    float4 sPos : TEXCOORD1;
    float2 Uv : TEXCOORD0;
    float4 Color : COLOR0;
    float Image : COLOR1;
    float LightLerp : TEXCOORD2;
};

struct PixelToFrame
{
    float4 Color : COLOR0;
};

float4 GetTexture(float2 texCoord, float img)
{
    texCoord = (texCoord * 0.25f) + imgageUV[round(img * 100.0f)];
    return partTexture.Sample(partTextureSampler, texCoord);
}

float2 GetTexCoords(float2 texCoord, float img)
{
    return (texCoord * 0.25f) + imgageUV[round(img * 100.0f)];
}

PixelInput VS(VertexTextureInstance input)
{
    PixelInput output;

    float4x4 world = transpose(input.instanceTransform);
    input.Position.xyz = world._41_42_43;

    float3 center = mul(input.Position, World).xyz;
    float3 eyeVector = center - CameraPosition();

    float3 finalPos = center;
    float3 sideVector, upVector;

    sideVector = normalize(cross(eyeVector, worldUp));
    upVector = normalize(cross(sideVector, eyeVector));

    finalPos += (input.Uv.x - 0.5f) * sideVector * world._13;
    finalPos += (0.5f - input.Uv.y) * upVector * (world._24);

    float4 finalPos4 = float4(finalPos, 1.0f);
    
    output.Position = mul(finalPos4, View);
    output.Position = mul(output.Position, Projection);

    output.sPos = output.Position;

    output.Uv = input.Uv;

    output.Color = float4(0.9f, 0.9f, 0.9f, 1.0f) * world._34;

    // Which sprite to draw...
    output.Image = world._12;

    // Alpha
    output.Color.a = world._23;

    // Misting
    float distVal   = length(abs(finalPos4.xyz - CameraPosition()));
    float distVal2  = length(abs(center - CameraPosition()));

    output.Color.a = (distVal  <= MistingDistance) ? (output.Color.a * distVal  / MistingDistance) : output.Color.a;
    output.Color.a = (distVal2 <= MistingDistance) ? (output.Color.a * distVal2 / MistingDistance) : output.Color.a;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 retColor = 0;

    float color = GetTexture(input.Uv, input.Image).rgb;

    float4 licol = lightColor;

    licol = (timeOfDay <= 12.0f) ? (licol * (timeOfDay / 12.0f)) : (licol * ((timeOfDay - 24.0f) / -12.0f));

    licol += 0.5f;

    retColor = input.Color * licol;
    retColor.a *= color;

    // Draw lighter as we go down the texture.
    retColor.a *= 1.0f - input.Uv.y;

    // Misting
    float distVal = input.sPos.z * 0.0025f;

    retColor.a *= saturate(distVal);

    return retColor;
}