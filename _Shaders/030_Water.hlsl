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
    float4 ShallowColor; //������
    float4 DeepColor; //������

    float FresnelBias; //����
    float FresnelPower; //����
    float FresnelAmount; //������
    float ShoreBlend; //���İ� ��ȭ����

    float2 OceanSize; //���ؽ��� ��üũ��
    float HeightRatio; //���� ������
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

struct Wave // 3�� ����� ����
{
    float Frequency; // �ĵ�
    float Amplitude; // ����
    float Phase; // ����
    float2 Direction; // ���� (x, z)
};

float EvaluateWave(Wave wave, float2 position, float time)
{
    //float s = sin(dot(wave.Direction, position) * wave.Frequency + time + wave.Phase);
    //float s = sin(dot(wave.Direction, position) * wave.Frequency + time) + wave.Phase;
	// ������ ��� �´°� ���� 
	// http://rosagigantea.tistory.com/334
    float s = sin(dot(wave.Direction, position) * wave.Frequency + time * wave.Phase);

    return wave.Amplitude * s;
}

PixelInput VS(VertexTexture input)
{
    PixelInput output;

	// wave ���� ���� �������� ������ ���� �־���
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

    // ��� ���� ��ȯ�� ���Ŀ� position�� ���� �ؾߵǱ� ��
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    return float4(0, 0, 0, 1);
    // ����� �ϱ� ���ؼ� normal�� ��ȯ���� ��������
    //return float4((input.Normal * 0.5f) + 0.5f, 1);
}