cbuffer VS_ViewProjection : register(b0) // register buffer 0번 쓰겠다는거
{
    matrix _view;
    matrix _projection;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}

cbuffer PS_Color : register(b0) // 버텍스 쉐이더랑 별개이므로 다시 0번부터
{
    float4 Color; 
}

struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : UV0;
};

struct PixelInput
{
    float4 position : SV_POSITION; // SV만 정해진 예약어 나머진 편한대로 쓰면 됨
    float2 uv : UV0;
};

SamplerState Sampler : register(s0); // sampler라서 버퍼 아니라 s0
// 기본꺼 써도 됨 안넣어져도 기본거 있음
Texture2D Map : register(t0); // texture라 t0
Texture2D Map2 : register(t1); // 2번째 texture
Texture2D Map3 : register(t2); // 3번째 texture

PixelInput VS(VertexInput input)
{
    PixelInput output;

    // 사실 월드 변환된 이후에 position에 따라 해야되긴 함
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 t = Map.Sample(Sampler, input.uv);
    float4 t2 = Map2.Sample(Sampler, input.uv);
    float4 alpha = Map3.Sample(Sampler, input.uv);

    return (1 - alpha.r) * t + t2 * alpha.r;

}