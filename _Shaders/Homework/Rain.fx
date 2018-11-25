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

float3 Velocity = float3(0, -100, 0);

float DrawDistance = 1000.0f;

float4 Color = float4(1, 1, 1, 1);
float3 Origin = float3(0, 0, 0);

float3 Size = float3(100, 100, 100);

struct VertexInput
{
    float4 Center : POSITION0;
    matrix World : INSTANCE0;
};

struct VertexOutput
{
    float4 Center : POSITION0;
    matrix World : MATRIX0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Center = input.Center;
    output.World = input.World;

    return output;
}

//-----------------------------------------------------------------------------
// Geometry Shader
//-----------------------------------------------------------------------------

struct GeometryOutput
{
    float4 Position : SV_POSITION0;
    float2 Uv : UV0;
    float Alpha : ALPHA0;
};

float2 TexCoord[4] =
{
    float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0)
};

[maxvertexcount(4)]
void GS(point VertexOutput input[1], inout TriangleStream<GeometryOutput> stream)
{
    float3 scale = input[0].World._11_22_33;
    float3 position = input[0].Center.xyz + input[0].World._41_42_43;

    float3 velocity = Velocity;
    velocity.xz /= scale.y * 0.1f;

    float time = Time * 1000.0f / 750.0f;

    float3 displace = time * velocity;

    position = Origin.xyz 
		+ (Size.xyz + (position + displace.xyz) % Size.xyz) % Size.xyz 
		- (Size.xyz * 0.5f);

    float3 axis = normalize(-velocity);
    float3 view = position - ViewPosition;
    float3 side = normalize(cross(axis, view));

    float4 v[4];

    GeometryOutput output;
	[unroll]
    for (int i = 0; i < 4; i++)
    {
        float3 halfWidth = (TexCoord[i].x - 0.5f) * side * scale.x;
        float3 halfHeight = (1.5f - TexCoord[i].y * 1.5f) * axis * scale.y;
        
        v[i] = float4(position + halfWidth + halfHeight, 1.0f);

        output.Position = mul(v[i], View);
        output.Position = mul(output.Position, Projection);

        output.Uv = TexCoord[i];

        float alpha = cos(Time + (position.x + position.z));
        alpha = saturate(1.5f + alpha / DrawDistance * 2);

        output.Alpha = 0.2f * saturate(1 - output.Position.z / DrawDistance) * alpha;

        stream.Append(output);
    }
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------

Texture2D Map;
SamplerState Sampler;

float4 PS(GeometryOutput input) : SV_TARGET
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
        SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));

        SetBlendState(BlendModeOn, float4(0, 0, 0, 0), 0xFFFFFFFF);
    }
}