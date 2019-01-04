#include "../000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader Scattering
//-----------------------------------------------------------------------------

//  상수는 static const로 선언
static const float PI = 3.14159265f;
static const float InnerRadius = 6356.7523142; //  지구 반지름
static const float OuterRadius = 6356.7523142 * 1.0157313; //  대기 반지름

static const float ESun = 20.0f; //  태양의 밝기
static const float Kr = 0.0025f; //  Rayleigh
static const float Km = 0.0010f; //  Mie

static const float KrESun = Kr * ESun;
static const float KmESun = Km * ESun;
static const float Kr4PI = Kr * 4.0f * PI;
static const float Km4PI = Km * 4.0f * PI;

static const float Scale = 1.0f / (OuterRadius - InnerRadius); //   비율 
static const float2 RayleighMieScaleHeight = { 0.25, 0.1 };

static const float g = -0.990f;
static const float g2 = -0.990f * -0.990f;
static const float Exposure = -2.0f; //  밝은 빛을 강조 시키기 위한 상수

struct PixelInput
{
    float4 Position : SV_POSITION0;
    float2 Uv : UV0;
    float3 oPosition : POSITION1;
};

PixelInput VS_Scattering(VertexTexture input)
{
    PixelInput output;
    
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.oPosition = -input.Position.xyz;
    output.Uv = input.Uv;

    return output;
}

//-----------------------------------------------------------------------------
// Pisxel Shader Scattering
//-----------------------------------------------------------------------------

// Phase 위상, 단계
float GetRayleighPhase(float c)
{
    return 0.75f * (1.0f + c);
}

float GetMiePhase(float c1, float c2)
{
    float3 result = 0;

    result.x = 1.5f * ((1.0f - g2) / (2.0f + g2));
    result.y = 1.0f + g2;
    result.z = 2.0f * g;
    return result.x * (1.0 + c2) / pow(result.y - result.z * c1, 1.5);
}

float3 HDR(float3 LDR)
{
    return 1.0f - exp(Exposure * LDR);
}

Texture2D RayleighMap;
Texture2D MieMap;
Texture2D StarMap;

SamplerState Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};

cbuffer PS_Scatter
{
    float StarIntensity = 0.0f;
}

float4 PS_Scattering(PixelInput input) : SV_TARGET
{
    float3 sunDirection = -normalize(LightDirection);

    float temp = dot(sunDirection, input.oPosition) / length(input.oPosition);
    float temp2 = temp * temp;

    float3 rSamples = RayleighMap.Sample(Sampler, input.Uv).rgb;
    float3 mSamples = MieMap.Sample(Sampler, input.Uv).rgb;

    float3 color = 0;
	
    color = GetRayleighPhase(temp2) * rSamples + GetMiePhase(temp, temp2) * mSamples;

    color = HDR(color);

    color += max(0, (1 - color)) * float3(0.05f, 0.05f, 0.1f);

    float intensity = saturate(StarIntensity);
    return float4(color, 1) + StarMap.Sample(Sampler, input.Uv) * intensity;
}

///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Vertex Shader Target
//-----------------------------------------------------------------------------

cbuffer PS_Target
{
    float3 WaveLength = float3(0.65f, 0.57f, 0.475f);
    int SampleCount = 20;

    float3 InvWaveLength;
    float3 WaveLengthMie;
}

struct PixelTargetInput
{
    float4 Position : SV_POSITION;
    float2 Uv : UV0;
};


PixelTargetInput VS_Target(VertexTexture input)
{
    PixelTargetInput output;
    
    //  NDC 공간
    output.Position = input.Position;
    output.Uv = input.Uv;
    
    return output;
}

//-----------------------------------------------------------------------------
// Pisxel Shader Target
//-----------------------------------------------------------------------------

struct PixelTargetOutput
{
    float4 RColor : SV_TARGET0;
    float4 MColor : SV_TARGET1;
};

float HitOuterSphere(float3 position, float3 direction)
{
    float3 light = -position;
    
    float b = dot(light, direction);
    float c = dot(light, light);

    float d = c - b * b;
    float q = sqrt(OuterRadius * OuterRadius - d);

    float t = b;
    t += q;

    return t;
}

float2 GetDensityRatio(float height)
{
    float altitude = (height - InnerRadius) * Scale;

    return exp(-altitude / RayleighMieScaleHeight.xy);
}

float2 GetDistance(float3 p1, float3 p2)
{
    float2 opticalDepth = 0;

    float3 temp = p2 - p1;
    float far = length(temp);
    float3 direction = temp / far;

    float sampleLength = far / SampleCount;
    float scaledLength = sampleLength * Scale;

    float3 sampleRay = direction * sampleLength;
    p1 += sampleRay * 0.5f;

    for (int i = 0; i < SampleCount; i++)
    {
        float height = length(p1);
        opticalDepth += GetDensityRatio(height);

        p1 += sampleRay;
    }
    
    return opticalDepth * scaledLength;
}

PixelTargetOutput PS_Target(PixelTargetInput input) : SV_TARGET
{
    PixelTargetOutput output;
    
    float2 uv = input.Uv;

    float3 pointPv = float3(0, InnerRadius + 1e-3f, 0.0f);
    float angleY = 100.0f * uv.x * PI / 180.0f;
    float angleXZ = PI * uv.y;

    float3 direction;
    direction.x = sin(angleY) * cos(angleXZ);
    direction.y = cos(angleY);
    direction.z = sin(angleY) * sin(angleXZ);
    direction = normalize(direction);

    float farPvPa = HitOuterSphere(pointPv, direction);
    float3 ray = direction;

    float3 pointP = pointPv;
    float sampleLength = farPvPa / SampleCount;
    float scaledLength = sampleLength * Scale;
    float3 sampleRay = ray * sampleLength;
    pointP += sampleRay * 0.5f;

    float3 sunDir = normalize(LightDirection);
    sunDir.y *= -1.0f;

    float3 rayleighSum = 0;
    float3 mieSum = 0;

    for (int i = 0; i < SampleCount; i++)
    {
        float pHeight = length(pointP);
        
        float2 densityRatio = GetDensityRatio(pHeight);
        densityRatio *= scaledLength;

        float2 viewerOpticalDepth = GetDistance(pointP, pointPv);

        float farPPc = HitOuterSphere(pointP, sunDir);
        float2 sunOpticalDepth = GetDistance(pointP, pointP + sunDir * farPPc);

        float2 opticalDepthP = sunOpticalDepth.xy + viewerOpticalDepth.xy;
        float3 attenuation = exp(-Kr4PI * InvWaveLength * opticalDepthP.x - Km4PI * opticalDepthP.y);

        rayleighSum += densityRatio.x * attenuation;
        mieSum += densityRatio.y * attenuation;

        pointP += sampleRay;
    }

    float3 rayleigh = rayleighSum * KrESun;
    float3 mie = mieSum * KmESun;

    rayleigh *= InvWaveLength;
    mie *= WaveLengthMie;

    output.RColor = float4(rayleigh, 1);
    output.MColor = float4(mie, 1);

    return output;
}

//-----------------------------------------------------------------------------
// States
//-----------------------------------------------------------------------------

DepthStencilState Depth
{
	DepthEnable = false;
    DepthWriteMask = ZERO;
};
//-----------------------------------------------------------------------------
// Techinques
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetDepthStencilState(Depth, 0);

        SetVertexShader(CompileShader(vs_5_0, VS_Scattering()));
        SetPixelShader(CompileShader(ps_5_0, PS_Scattering()));
    }

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Target()));
        SetPixelShader(CompileShader(ps_5_0, PS_Target()));
    }
}