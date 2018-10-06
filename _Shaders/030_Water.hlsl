#include "000_Header.hlsl"

cbuffer VS_Ocean : register(b10)
{
    float WaveFrequency;
    float WaveAmplitude;

    float2 TextureScale;

    float2 BumpSpeed;
    float BumpHeight;
    float RunningTime;
}

cbuffer PS_Ocean : register(b10)
{
    float4 ShallowColor; //얕은색
    float4 DeepColor; //깊은색

    float FresnelBias; //편향
    float FresnelPower; //강도
    float FresnelAmount; //보정값
    float ShoreBlend; //알파값 변화정도

    float2 OceanSize; //버텍스의 전체크기
    float HeightRatio; //높이 보정값
}

struct PixelInput
{
    float4 Position : SV_POSITION;
    float4 oPosition : POSITION0;
    float4 wPosition : POSITION1;
    float2 TexScale : TEXSCALE0;
    float3 Normal : NORMAL0;
    float3 View : VIEW0;
    
    float2 Bump[3] : BUMP0; // Normal 의미
    float3 Tangent[3] : TANGENT0;
};

struct Wave // 3개 만들어 쓸꺼
{
    float Frequency; // 파동
    float Amplitude; // 진폭
    float Phase; // 위상
    float2 Direction; // 방향 (x, z)
};

float EvaluateWave(Wave wave, float2 position, float time)
{
    //float s = sin(dot(wave.Direction, position) * wave.Frequency + time + wave.Phase);
    float s = sin(dot(wave.Direction, position) * wave.Frequency + time) + wave.Phase;
	// 이쪽이 계산 맞는거 같음 -> 이 사이트 부분이 잘못된거 같음 (준형이형 얘기 듣고보니)
	// http://rosagigantea.tistory.com/334
    //float s = sin(dot(wave.Direction, position) * wave.Frequency + time * wave.Phase);

    return wave.Amplitude * s;
}

float EvaluateDifferent(Wave wave, float2 position, float time)
{
    float s = cos(dot(wave.Direction, position) * wave.Frequency + time) + wave.Phase;

    return wave.Amplitude * wave.Frequency * s;
}

PixelInput VS(VertexTexture input)
{
    PixelInput output;

	// wave 넣은 값은 선생님이 임의의 값을 넣어줌
    Wave wave[3] =
    {
        0.0f, 0.0f, 0.50f, float2(-1.0f, 0.0f),
        0.0f, 0.0f, 1.30f, float2(-0.7f, -0.7f),
        0.0f, 0.0f, 0.25f, float2(0.2f, 0.1f),
    };

    wave[0].Frequency = WaveFrequency;
    wave[0].Amplitude = WaveAmplitude;
    wave[1].Frequency = WaveFrequency * 2.0f;
    wave[1].Amplitude = WaveAmplitude * 0.5f;
    wave[2].Frequency = WaveFrequency * 3.0f;
    wave[2].Amplitude = WaveAmplitude * 1.0f;

    float ddx = 0, ddy = 0;
    for (int i = 0; i < 3; i++)
    {
        input.Position.y += EvaluateWave(wave[i], input.Position.xz, RunningTime);

        float diff = EvaluateDifferent(wave[i], input.Position.xz, RunningTime);
        ddx += diff * wave[i].Direction.x;
        ddy += diff * wave[i].Direction.y;
    }

    float3 T = float3(1, ddx, 0);
    float3 B = float3(-ddx, 1, -ddy);
    float3 N = float3(0, ddy, 1);

    float3x3 matTangent = float3x3
	(
		normalize(T) * BumpHeight,
		normalize(B) * BumpHeight,
		normalize(N)
	);
	
    output.Tangent[0] = mul(World[0].xyz, matTangent);
    output.Tangent[1] = mul(World[1].xyz, matTangent);
    output.Tangent[2] = mul(World[2].xyz, matTangent);

    output.TexScale = input.Uv * TextureScale;

    float tempTime = fmod(RunningTime, 100);
    output.Bump[0] = output.TexScale + tempTime * BumpSpeed;
    output.Bump[1] = output.TexScale * 2.0f + tempTime * BumpSpeed * 4.0f;
    output.Bump[2] = output.TexScale * 4.0f + tempTime * BumpSpeed * 8.0f;

    output.oPosition = input.Position;

    output.Position = mul(input.Position, World);
    output.wPosition = output.Position;
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

	output.View = ViewInverse._41_42_43 - output.wPosition.xyz;

    return output;
}

Texture2D HeightMap : register(t5);
SamplerState HeightSampler : register(s5);

bool WithInBound(float3 position)
{
    return (position.x > 0.0f && position.z > 0.0f 
	&& position.x < OceanSize.x && position.z < OceanSize.y);
}

float EvaluateShoreBlend(float3 position)
{
    float2 temp = float2(position.x / OceanSize.x, position.z / OceanSize.y);
    float color = HeightMap.Sample(HeightSampler, temp).r / HeightRatio;

    return 1.0f - color * ShoreBlend; // ShoreBlend 높이에 대한 비율
}

float4 PS(PixelInput input) : SV_TARGET
{
	// 색상값 축으로 쓰기 위해 2.0f 곱하고 -1.0f 한거
    float4 t0 = NormalMap.Sample(NormalSampler, input.Bump[0]) * 2.0f - 1.0f;
    float4 t1 = NormalMap.Sample(NormalSampler, input.Bump[1]) * 2.0f - 1.0f;
    float4 t2 = NormalMap.Sample(NormalSampler, input.Bump[2]) * 2.0f - 1.0f;

    //float3 normal = t0.xyz;
	float3 normal = t0.xyz + t1.xyz + t2.xyz;

    float3x3 matTangent;
    matTangent[0] = input.Tangent[0];
    matTangent[1] = input.Tangent[1];
    matTangent[2] = input.Tangent[2];
	
    normal = mul(normal, matTangent);
    normal = normalize(normal);

    float4 color = 0;
    float facing = 1.0f - saturate(dot(input.View, normal));
    float fresnel = FresnelBias + (1.0f - FresnelBias) * pow(facing, FresnelPower);

    float alpha = 0;
    color = lerp(DeepColor, ShallowColor, facing);
    if (ShoreBlend > 0 && WithInBound(input.oPosition.xyz))
    {
        alpha = EvaluateShoreBlend(input.oPosition.xyz);
        color.rgb = lerp(2, color.rgb, color.a);
    }
    color.rgb = color.rgb * Ambient.rgb * fresnel;

    DiffuseLighting(color, Diffuse, normal);
    SpecularLighting(color, normal, input.View);

    return float4(color.rgb, alpha);

    //return float4(0, 0, 0, 1);
    // 디버깅 하기 위해서 normal을 반환형인 색상값으로
    //return float4((input.Normal * 0.5f) + 0.5f, 1);
}