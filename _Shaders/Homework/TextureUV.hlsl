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

cbuffer PS_UV : register(b1)
{
    float UV;
}

struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : UV0; // 녹색 글씨이 Semantic 어떤 녀석으로 쓸지
    // 9엔 의미 정해져있었는데 10부턴 의미 없음 같은 종류로만 관리하면 됨
};

// uv는 픽셀 쉐이더에서 처리
// 샘플링 얘기하면서 설명해주실꺼

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : UV0;
};

SamplerState Sampler : register(s0); // sampler라서 버퍼 아니라 s0
// 기본꺼 써도 됨 안넣어져도 기본거 있음
Texture2D Map : register(t0); // texture라 t0
Texture2D Map2 : register(t1); // 2번째 texture

PixelInput VS(VertexInput input)
{
    PixelInput output;
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = Map2.Sample(Sampler, input.uv); 

    // 픽셀 쉐이더에선 분기나 반복 안쓰는게 좋음
    // 이유에 대해선 나중에 설명해주실꺼
    float x = input.uv.x;

    if(x < UV)
        color = Map.Sample(Sampler, input.uv);

    return color;
}