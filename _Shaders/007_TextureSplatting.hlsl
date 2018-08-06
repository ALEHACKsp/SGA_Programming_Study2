cbuffer VS_ViewProjection : register(b0) // register buffer 0�� ���ڴٴ°�
{
    matrix _view;
    matrix _projection;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}

cbuffer PS_Color : register(b0) // ���ؽ� ���̴��� �����̹Ƿ� �ٽ� 0������
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
    float4 position : SV_POSITION; // SV�� ������ ����� ������ ���Ѵ�� ���� ��
    float2 uv : UV0;
};

SamplerState Sampler : register(s0); // sampler�� ���� �ƴ϶� s0
// �⺻�� �ᵵ �� �ȳ־����� �⺻�� ����
Texture2D Map : register(t0); // texture�� t0
Texture2D Map2 : register(t1); // 2��° texture
Texture2D Map3 : register(t2); // 3��° texture

PixelInput VS(VertexInput input)
{
    PixelInput output;

    // ��� ���� ��ȯ�� ���Ŀ� position�� ���� �ؾߵǱ� ��
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