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
};

struct PixelInput
{
    float4 position : SV_POSITION; // SV만 정해진 예약어 나머진 편한대로 쓰면 됨
    float4 color : COLOR0;
};

SamplerState Sampler : register(s0); // sampler라서 버퍼 아니라 s0
// 기본꺼 써도 됨 안넣어져도 기본거 있음
Texture2D Map : register(t0); // texture라 t0
Texture2D Map2 : register(t1); // 2번째 texture

PixelInput VS(VertexInput input)
{
    PixelInput output;

    float4 color = float4(1, 1, 1, 1);

    if (input.position.y > 10)
        color = float4(1, 1, 0, 1);
    if (input.position.y > 20)
        color = float4(1, 0, 0, 1);
    if (input.position.y > 30)
        color = float4(0, 1, 0, 1);
    if (input.position.y > 50)
        color = float4(0, 0, 1, 1);
    output.color = color;

    // 사실 월드 변환된 이후에 position에 따라 해야되긴 함
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    return input.color;
}