cbuffer VS_ViewProjection : register(b0) // register buffer 0번 쓰겠다는거
{
    matrix View;
    matrix Projection;
}

cbuffer VS_World : register(b1)
{
    matrix World;
}

cbuffer VS_Bones : register(b2)
{
    matrix Bones[128];
}

cbuffer VS_BoneIndex : register(b3)
{
    int BoneIndex;
}

cbuffer PS_Light : register(b0) // 버텍스 쉐이더랑 별개이므로 다시 0번부터
{
    float3 Direction;
}

// material 재질 의미
cbuffer PS_Material : register(b1)
{
    float4 Diffuse;
}

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSampler : register(s0);

struct VertexColor
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
};

struct VertexColorNormal
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
    float3 Normal : NORMAL0;
};

struct VertexTexture
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
};

struct VertexTextureNormal
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
};

struct VertexColorTextureNormal
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
};

// 색상만을 쓸 때
void DiffuseLighting(inout float4 color, float3 normal)
{
    //float3 light = _direction * -1;
    // 빛의 강도
    // saturate 0 ~ 1까지 제한해주는 함수
    float intensity = saturate(dot(normal, -Direction));

    color = color + Diffuse * intensity;
}

void DiffuseLighting(inout float4 color, float4 diffuse, float3 normal)
{
    //float3 light = _direction * -1;
    // 빛의 강도
    // saturate 0 ~ 1까지 제한해주는 함수
    float intensity = saturate(dot(normal, -Direction));

    color = color + Diffuse * diffuse * intensity;
}

// inout은 c++ reference 같은거라고 보면 됨
// in out도 있는데 생략하는데 엔진팀에선 꼭 적어둬야한다고 하심
// 텍스처 쓸 때
void DiffuseLight(inout float4 color, float4 diffuse, float3 normal)
{
    //float3 light = _direction * -1;
    // 빛의 강도
    // saturate 0 ~ 1까지 제한해주는 함수
    float intensity = saturate(dot(normal, -Direction));

    color = color + Diffuse * diffuse * intensity;
}