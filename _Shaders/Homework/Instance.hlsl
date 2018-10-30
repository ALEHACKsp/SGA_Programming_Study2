cbuffer VS_ViewProjection : register(b0) // register buffer 0번 쓰겠다는거
{
    matrix _view;
    matrix _projection;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}

cbuffer PS_Color : register(b1) // 버텍스 쉐이더랑 별개이므로 다시 0번부터
{
    float4 Color; 
}

struct VertexInput
{
    float4 position : POSITION0;
    
    float4x4 instanceWorld : WORLD0;
	//float4 instanceColor : COLOR0;
    //float4 instancePosition : INSTANCE0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

PixelInput VS(VertexInput input)
{ 
    //input.position.xyz += input.instancePosition.xyz;

    PixelInput output;
    //output.position = mul(input.position, _world);
    output.position = mul(input.position, input.instanceWorld);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    //output.color = input.instanceColor;
    output.color = float4(1, 1, 1, 1);

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    //return Color;
    //return float4(1, 1, 1, 1);
    return input.color;
}