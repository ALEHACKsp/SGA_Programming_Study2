#include "../000_Header.hlsl"

cbuffer PS_Color : register(b2) // ���ؽ� ���̴��� �����̹Ƿ� �ٽ� 0������
{
    float4 LightColor; 
    float timeOfDay;
}

static float3 worldUp = float3(0, 1, 0);

//static float timeOfDay = 12;

static float MistingDistance = 50;

static const float2 imgageUV[16] =
{
    float2(0, 0), float2(.25, 0), float2(.50, 0), float2(.75, 0),
		float2(0, .25), float2(.25, .25), float2(.50, .25), float2(.75, .25),
		float2(0, .50), float2(.25, .50), float2(.50, .50), float2(.75, .50),
		float2(0, .75), float2(.25, .75), float2(.50, .75), float2(.75, .75)
};

Texture2D CloudMap : register(t10);
SamplerState CloudSampler : register(s10);

struct PixelInput
{
    float4 Position : SV_POSITION;
    float4 sPosition : POSITION1;

    float2 Uv : TEXCOORD0;
    float4 Color : COLOR0;
    float Image : COLOR1;
    float LightLerp : TEXCOORD1;
};

float4 GetTexture(float2 uv, float img)
{
    uv = (uv * 0.25f) + imgageUV[round(img * 100.0f)];
    return CloudMap.Sample(CloudSampler, uv);
}

float2 GetUV(float2 uv, float img)
{
    return (uv * 0.25f) + imgageUV[round(img * 100.0f)];
}

PixelInput VS(VertexTexture input, float4x4 instanceWorld : INSTANCE0)
{ 
    PixelInput output;

    float4x4 world = transpose(instanceWorld);
    input.Position.xyz = float3(world._41, world._42, world._43); 

    float3 center = mul(input.Position, World).xyz; // ������ �߽��� 
    float3 eyeVector = center - CameraPosition(); // ī�޶�κ��� �߽��������� ����

    float3 finalPos = center;
    float3 sideVector; // ������ (right��� ����)
    float3 upVector; // Up ����

    sideVector = normalize(cross(eyeVector, worldUp)); // ũ�ν��ؼ� �� �� ���ϱ�
    upVector = normalize(cross(sideVector, eyeVector));

	// uv�� 0,1 �������� ���� ó���� 0,1�� �־��ִ� (���� ���鶧)
    finalPos += (input.Uv.x - 0.5f) * sideVector * world._13; // �������κ��� ���������� ���� ��ġ �̵�
    finalPos += (0.5f - input.Uv.y) * upVector * world._24; // �������κ��� ���������� ���� ��ġ �̵�

    float4 finalPos4 = float4(finalPos, 1);

    output.Position = mul(finalPos4, View);
    output.Position = mul(output.Position, Projection);

    output.sPosition = output.Position;

    output.Uv = input.Uv;

    output.Color = float4(0.9f, 0.9f, 0.9f, 1.0f) * world._34;

	// Which sprite to draw...
    output.Image = world._12;

	// Alpha
    output.Color.a = world._23;

	// Misting
	// ��������� ������� �ϴ°� ����
	// ������ �ִ��� �߽��� ������� �� �վ �ι� ����غ��°�
    float distVal = length(abs(finalPos - CameraPosition()));
    float distVal2 = length(abs(center - CameraPosition()));

    if (distVal <= MistingDistance)
		output.Color.a *= distVal / MistingDistance;

    if (distVal2 <= MistingDistance)
        output.Color.a *= distVal2 / MistingDistance;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 Color;

    float color = GetTexture(input.Uv, input.Image).rgb;

    float4 licol = LightColor;
	
    if (timeOfDay <= 12.0f)
        licol *= timeOfDay / 12.0f;
	else
        licol *= (timeOfDay - 24.0f) / -12.0f;

    licol += 0.5f;

    Color = input.Color * licol;

    Color.a *= color;

	// Draw lighter as we go down the texture.
    Color.a *= 1.0f - input.Uv.y;

	// Misting
    float distVal = input.sPosition.z * 0.0025f;

    Color.a *= saturate(distVal);

    //return float4(0, 0, 1, 1);
    //return color;
    //Color.a = 1.0f;

    return Color;

}