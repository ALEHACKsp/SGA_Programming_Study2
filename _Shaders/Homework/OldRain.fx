#include "../000_Header.fx"

//-----------------------------------------------------------------------------
// States
//-----------------------------------------------------------------------------

BlendState BlendModeOn // 아무것도 설정안하면 기본값됨
{
    BlendEnable[0] = true;
    DestBlend = INV_SRC_ALPHA;
    SrcBlend = SRC_ALPHA;
    BlendOp = ADD;
	
    DestBlendAlpha = ZERO;
    SrcBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
};

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

float3 Velocity = float3(0, -200, 0);

float DrawDistance = 1000.0f;

float4 Color = float4(1, 1, 1, 1);
float3 Origin = float3(0, 0, 0);

float3 Size = float3(100, 100, 100);

struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : UV0;
    float2 Scale : SCALE0;
};

struct VertexOutput
{
    float4 Position : SV_POSITION0;
    float2 Uv : UV0;
    float Alpha : ALPHA0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    float3 velocity = Velocity;
	// y 스케일에 따라 xz의 크기를 조정해서 보여주려고 하는거
    velocity.xz /= input.Scale.y * 0.1f;

    float time = Time * 1000.0f / 750.0f;

	// 위치 이동 시키는 거
    float3 displace = time * velocity;

    input.Position.xyz = Origin.xyz 
		+ (Size.xyz + (input.Position.xyz + displace.xyz) % Size.xyz) % Size.xyz 
		- (Size.xyz * 0.5f);

    float4 position = mul(input.Position, World);
    float3 axis = normalize(-velocity);
    float3 view = position.xyz - ViewPosition;
    float3 side = normalize(cross(axis, view));

    position.xyz += (input.Uv.x - 0.5f) * side * input.Scale.x;
    position.xyz += (1.5f - input.Uv.y * 1.5f) * axis * input.Scale.y;

    position.w = 1.0f;

    output.Position = mul(position, View);
    output.Position = mul(output.Position, Projection);

    output.Uv = input.Uv;

    float alpha = cos(Time + (input.Position.x + input.Position.z));
    alpha = saturate(1.5f + alpha / DrawDistance * 2);

    output.Alpha = 0.2f * saturate(1 - output.Position.z / DrawDistance) * alpha;

    return output;
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------

Texture2D Map;
SamplerState Sampler;

float4 PS(VertexOutput input) : SV_TARGET
{
    float4 color = Map.Sample(Sampler, input.Uv);
    color.rgb *= Color.xyz * (1 + input.Alpha) * 2.0f;
    color.a *= input.Alpha;

    //return float4(1, 0, 0, 1);
    return color;
}

//-----------------------------------------------------------------------------
// Techinque
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        //SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));

        SetBlendState(BlendModeOn, float4(0, 0, 0, 0), 0xFFFFFFFF);
    }
}