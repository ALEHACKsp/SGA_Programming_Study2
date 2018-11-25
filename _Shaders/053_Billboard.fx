#include "000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

struct VertexOutput
{
    float4 Center : POSITION0;
    float2 Size : SIZE0;
    //uint PrimitiveID : SV_PrimitiveID;
};

VertexOutput VS(VertexSize input)
{
    VertexOutput output;

    output.Center = input.Position;
    output.Size = input.Size;

    return output;
}

//-----------------------------------------------------------------------------
// Geometry Shader
//-----------------------------------------------------------------------------

struct GeometryOutput
{
    float4 Position : SV_POSITION0;
    //float3 wPosition : POSITION1;
    //float3 Normal : NORMAL0;
    float2 Uv : UV0;
    uint PrimitiveID : SV_PrimitiveID;
};

static const float2 TexCoord[4] = { float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0) };

[maxvertexcount(4)]
void GS(point VertexOutput input[1], uint PrimitiveID : SV_PrimitiveID, inout TriangleStream<GeometryOutput> stream)
{
    float3 up = float3(0, 1, 0);
    float3 look = ViewPosition - input[0].Center.xyz;
    look.y = 0.0f; // 수직 고정
    look = normalize(look);

    float3 right = cross(up, look);

    float halfWidth = 0.5f * input[0].Size.x;
    float halfHeight = 0.5f * input[0].Size.y;

    float4 v[4];
    v[0] = float4(input[0].Center.xyz + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(input[0].Center.xyz + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(input[0].Center.xyz - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(input[0].Center.xyz - halfWidth * right + halfHeight * up, 1.0f);

    GeometryOutput output;
	[unroll]
    for (int i = 0; i < 4; i++)
    {
        output.Position = mul(v[i], View);
        output.Position = mul(output.Position, Projection);
        //output.wPosition = v[i];
		
        //output.Normal = look;
        output.Uv = TexCoord[i];
        output.PrimitiveID = PrimitiveID;

        stream.Append(output);
    }
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------

Texture2DArray Map;
SamplerState Sampler;

float4 PS(GeometryOutput input) : SV_TARGET
{
    float3 uvw = float3(input.Uv, input.PrimitiveID % 6);

    float4 color = Map.Sample(Sampler, uvw);

    clip(color.a - 0.5f); // 픽셀 빼기 하는거 알파 안쓰고 이렇게 했엇음

    return color;
}

//-----------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------
technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(CompileShader(gs_5_0, GS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}