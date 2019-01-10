#include "../000_Header.fx"

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

struct VertexInput
{
    float4 Center : POSITION0;

    matrix World : INSTANCE0;
    int TextureId : INSTANCE4;
	
    uint InstanceId : SV_INSTANCEID0;
};

struct VertexOutput
{
    float4 Center : POSITION0;

    matrix World : MATRIX0;
    int TextureId : TEXTURE0;

    uint InstanceId : UINT0;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Center = input.Center;
    output.World = input.World;

    output.TextureId = input.TextureId;

    output.InstanceId = input.InstanceId;

    return output;
}

//-----------------------------------------------------------------------------
// Geometry Shader
//-----------------------------------------------------------------------------

float3 WorldTangent(float3 tangent, matrix world)
{
    tangent = normalize(mul(tangent, (float3x3) world));

    return tangent;
}

struct GeometryOutput
{
    float4 Position : SV_POSITION0;
    
	float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;

    float2 Uv : UV0;

    int TextureId : TEXTURE0;

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
    look.y = 0.0f; // 수직 고정
    look = normalize(look);

    float3 right = cross(up, look);

    float halfWidth = 0.5f * input[0].World._11;
    float halfHeight = 0.5f * input[0].World._22;

    float4 v[4];
    v[0] = float4(position + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(position + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(position - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(position - halfWidth * right + halfHeight * up, 1.0f);

    GeometryOutput output;
	[unroll]
    for (int i = 0; i < 4; i++)
    {
        //output.Position = mul(v[i], input[0].World);
        //output.Position = mul(output.Position, View);
        
		output.Position = mul(v[i], View);
        output.Position = mul(output.Position, Projection);

        output.Normal = look;
        output.Tangent = right;

        output.Uv = TexCoord[i];

        output.InstanceId = input[0].InstanceId;

        output.TextureId = input[0].TextureId;

        stream.Append(output);
    }
}

// heightmap 적용
Texture2D HeightMap;
SamplerState HeightMapSampler
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
};
float MaxHeight;

[maxvertexcount(4)]
void GS2(point VertexOutput input[1], inout TriangleStream<GeometryOutput> stream)
{
    float3 up = float3(0, 1, 0);
    float3 position = input[0].Center.xyz + input[0].World._41_42_43;

    float2 uv = 0;
	uv.x = (input[0].World._41 + 512.0f) / 1024.0f;
    uv.y = (512.0f - input[0].World._43) / 1024.0f;

    position.y += HeightMap.SampleLevel(HeightMapSampler, uv, 0).r;

    float3 look = ViewPosition - position;
    look.y = 0.0f; // 수직 고정
    look = normalize(look);

    float3 right = cross(up, look);

    float halfWidth = 0.5f * input[0].World._11;
    float halfHeight = 0.5f * input[0].World._22;

    float4 v[4];
    v[0] = float4(position + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(position + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(position - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(position - halfWidth * right + halfHeight * up, 1.0f);

	// 정규 직교
    float3 e0 = v[1] - v[0];
    float3 e1 = v[2] - v[0];

    float u0 = TexCoord[1].x - TexCoord[0].x;
    float u1 = TexCoord[2].x - TexCoord[0].x;
    float v0 = TexCoord[1].y - TexCoord[0].y;
    float v1 = TexCoord[2].y - TexCoord[0].y;
    float r = 1.0f / (u0 * v1 - v0 * u1);

	// tangent 벡터
    float3 tangent;
    tangent.x = r * (v1 * e0.x - v0 * e1.x);
    tangent.y = r * (v1 * e0.y - v0 * e1.y);
    tangent.z = r * (v1 * e0.z - v0 * e1.z);

    GeometryOutput output;
	[unroll]
    for (int i = 0; i < 4; i++)
    {
        //output.Position = mul(v[i], input[0].World);
        //output.Position = mul(output.Position, View);
        
        output.Position = mul(v[i], View);
        output.Position = mul(output.Position, Projection);

        output.Normal = look;

        output.Tangent = normalize(tangent);

        output.Uv = TexCoord[i];

        output.InstanceId = input[0].InstanceId;

        output.TextureId = input[0].TextureId;

        stream.Append(output);
    }
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------

Texture2DArray Maps;
SamplerState Sampler;

Texture2DArray NormalMaps;

uint instanceCount;
float factor = 0.95f;

void NormalMapping(inout float4 color, float4 normalMap, float3 normal, float3 tangent)
{
    float3 N = normal; // Z축이랑 매핑됨
    float3 T = normalize(tangent - dot(tangent, N) * N); // X 이 식이 그람슈미트 식
    float3 B = cross(T, N); // Y

    float3x3 TBN = float3x3(T, B, N);

	// rgb 0~1 방향으로 만드는거
    float3 coord = 2.0f * normalMap - 1.0f;
    float3 bump = mul(coord, TBN); // max에선 normal mapping을 bump mapping이라 부름

    float intensity = saturate(dot(bump, -LightDirection));
    color = color * intensity;
}

float4 PS(GeometryOutput input) : SV_TARGET
{
    float3 uvw = float3(input.Uv, input.TextureId);

    float4 color = Maps.Sample(Sampler, uvw);

    float alpha = color.a;

    float4 normal = NormalMaps.Sample(Sampler, uvw);

    if (length(normal) > 0)
        NormalMapping(color, normal, input.Normal, input.Tangent);

    //color *= dot(-LightDirection, input.Normal);

    color.a = alpha;

    clip(color.a - 0.5f);

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

    pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(CompileShader(gs_5_0, GS2()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}