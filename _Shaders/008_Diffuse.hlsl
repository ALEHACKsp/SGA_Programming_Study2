cbuffer VS_ViewProjection : register(b0) // register buffer 0�� ���ڴٴ°�
{

    matrix _view;

    matrix _projection;

}



cbuffer VS_World : register(b1)
{

    matrix _world;

}



cbuffer PS_Light : register(b0) // ���ؽ� ���̴��� �����̹Ƿ� �ٽ� 0������
{

    float3 _direction;

}



struct VertexInput
{

    float4 position : POSITION0;

    float2 uv : UV0;

    float3 normal : NORMAL0;

};



struct PixelInput
{

    float4 position : SV_POSITION; // SV�� ������ ����� ������ ���Ѵ�� ���� ��

    float2 uv : UV0;

    float3 normal : NORMAL0;

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



    output.normal = mul(input.normal, (float3x3) _world);



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



float4 PS_Diffuse(PixelInput input) : SV_TARGET
{

    float3 light = _direction * -1;

    // ���� ����

    // saturate 0 ~ 1���� �������ִ� �Լ�

    float intensity = saturate(dot(input.normal, light));



    float4 t = Map.Sample(Sampler, input.uv);

    float4 t2 = Map2.Sample(Sampler, input.uv);

    float4 alpha = Map3.Sample(Sampler, input.uv);



    float4 ambient = (1 - alpha.r) * t + t2 * alpha.r;



    return ambient * intensity;

}