#include "000_Header2.hlsl"

struct VertexOutput
{
    float4 Position : POSITION;
    float2 Uv : UV0;
};

VertexOutput VS(VertexTexture input)
{
    VertexOutput output;

    output.Position = input.Position;
    output.Uv = input.Uv;

    return output;
}

///////////////////////////////////////////////////////////////////////////////

struct GeometryOutput
{
    float4 Position : SV_POSITION;
    float3 wPosition : POSITION0;
    float2 Uv : UV0;
};

// 실제 자르는 연산 사실 텍셀레이션에서 함
void SubDivision(VertexOutput vertices[3], out VertexOutput outVertices[6])
{
    VertexOutput m[3];

    m[0].Position = (vertices[0].Position + vertices[1].Position) * 0.5f;
    m[1].Position = (vertices[1].Position + vertices[2].Position) * 0.5f;
    m[2].Position = (vertices[2].Position + vertices[0].Position) * 0.5f;

    //m[0].Position = normalize(m[0].Position);
    //m[1].Position = normalize(m[1].Position);
    //m[2].Position = normalize(m[2].Position);

    m[0].Uv = (vertices[0].Uv + vertices[1].Uv) * 0.5f;
    m[1].Uv = (vertices[1].Uv + vertices[2].Uv) * 0.5f;
    m[2].Uv = (vertices[2].Uv + vertices[0].Uv) * 0.5f;

    outVertices[0] = vertices[0];
    outVertices[1] = m[0];
    outVertices[2] = m[2];
    outVertices[3] = m[1];
    outVertices[4] = vertices[2];
    outVertices[5] = vertices[1];
}

[maxvertexcount(8)]
void GS(triangle VertexOutput input[3], inout TriangleStream<GeometryOutput> stream)
{
    VertexOutput v[6];
    SubDivision(input, v); // 자르는 함수

    GeometryOutput output[6];
	[unroll]
    for (int i = 0; i < 6; i++)
    {
        output[i].Position = mul(v[i].Position, World);
        output[i].Position = mul(output[i].Position, View);
        output[i].Position = mul(output[i].Position, Projection);

        output[i].wPosition = mul(v[i].Position, World);

        output[i].Uv = v[i].Uv;
    }

	[unroll]
    for (int k = 0; k < 5; k++)
        stream.Append(output[k]);

    stream.RestartStrip();

    stream.Append(output[1]);
    stream.Append(output[5]);
    stream.Append(output[3]);
}

///////////////////////////////////////////////////////////////////////////////

float4 PS(GeometryOutput input) : SV_TARGET
{
    return float4(1, 0, 0, 1);
}