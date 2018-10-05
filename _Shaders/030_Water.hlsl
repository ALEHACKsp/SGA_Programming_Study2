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
    
    float2 Bump[3] : BUMP0;
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
    //float s = sin(dot(wave.Direction, position) * wave.Frequency + time) + wave.Phase;
	// 이쪽이 계산 맞는거 같음 
	// http://rosagigantea.tistory.com/334
    float s = sin(dot(wave.Direction, position) * wave.Frequency + time * wave.Phase);

    return wave.Amplitude * s;
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

    for (int i = 0; i < 3; i++)
        input.Position.y += EvaluateWave(wave[i], input.Position.xz, RunningTime);

    output.oPosition = input.Position;

    // 사실 월드 변환된 이후에 position에 따라 해야되긴 함
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    return float4(0, 0, 0, 1);
    // 디버깅 하기 위해서 normal을 반환형인 색상값으로
    //return float4((input.Normal * 0.5f) + 0.5f, 1);
}