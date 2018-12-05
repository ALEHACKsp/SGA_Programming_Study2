#include "../000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

struct VertexInput
{
    float4 Center : POSITION0;

    matrix World : INSTANCE0;
    float MoveSpeed : INSTANCE4;

    uint InstanceId : SV_INSTANCEID0;
};

struct VertexOutput
{
    float4 Center : POSITION0;

    matrix World : MATRIX0;
    float MoveSpeed : FLOAT0;

    uint InstanceId : UINT0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Center = input.Center;
    output.World = input.World;
    output.MoveSpeed = input.MoveSpeed;
    output.InstanceId = input.InstanceId;

    return output;
}

//-----------------------------------------------------------------------------
// Geometry Shader
//-----------------------------------------------------------------------------

struct GeometryOutput
{
    float4 Position : SV_POSITION0;
    //float3 Normal : NORMAL0;
    float2 Uv : UV0;
    uint InstanceId : UINT0;
};

float2 TexCoord[4] =
{
    float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0)
};

[maxvertexcount(4)]
void GS(point VertexOutput input[1], inout TriangleStream<GeometryOutput> stream)
{
    float3 up = float3(0, 1, 0);
    float3 position = input[0].Center.xyz + input[0].World._41_42_43;
    float3 look = ViewPosition - position;
    look.y = 0.0f; // ���� ����
    look = normalize(look);

    float3 right = cross(up, look);

    float halfWidth = 0.5f * input[0].World._11;
    float halfHeight = 0.5f * input[0].World._22;

    float4 v[4];
    v[0] = float4(position + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(position + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(position - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(position - halfWidth * right + halfHeight * up, 1.0f);

    v[1].x += cos(Time * input[0].MoveSpeed);
    v[3].x += cos(Time * input[0].MoveSpeed);

    GeometryOutput output;
	[unroll]
    for (int i = 0; i < 4; i++)
    {
        //output.Position = mul(v[i], input[0].World);
        //output.Position = mul(output.Position, View);
        
		output.Position = mul(v[i], View);
        output.Position = mul(output.Position, Projection);

        //output.Normal = look;
        output.Uv = TexCoord[i];

        output.InstanceId = input[0].InstanceId;

        stream.Append(output);
    }
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------

Texture2DArray Maps;
SamplerState Sampler;

uint instanceCount;
float factor = 0.95f;

float4 PS(GeometryOutput input) : SV_TARGET
{
    float3 uvw = float3(input.Uv, (input.InstanceId / (uint) (instanceCount * factor)) % 2);

    float4 color = Maps.Sample(Sampler, uvw);

    clip(color.a - 0.5f);

    //return float4(input.InstanceId % 2, 0, 0, 1);
    //return float4(input.Normal, 1);
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
    }
}