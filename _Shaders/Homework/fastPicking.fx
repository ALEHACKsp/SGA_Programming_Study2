#include "../000_Header.fx"

cbuffer CB_Terrain
{
    float4 FogColor;
    float FogStart;
    float FogRange;

    float MinDistance;
    float MaxDistance;
    float MinTessellation;
    float MaxTessellation;

    float TexelCellSpaceU;
    float TexelCellSpaceV;
    float WorldCellSpace;
	
    float2 TexScale = 66.0f;
    float CB_Terrain_Padding;

    float4 WorldFrustumPlanes[6];
};

Texture2D HeightMap;
SamplerState HeightMapSampler
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
};

float Width;
float Height;
float MaxHeight;

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

// Terrain

struct VertexInput_Terrain
{
    float4 Position : Position0;
    float2 Uv : Uv0;
    float2 BoundsY : BoundsY0;
};

struct VertexOutput_Terrain
{
    float4 Position : Position0;
    float2 Uv : Uv0;
    float2 BoundsY : BoundsY0;
};

VertexOutput_Terrain VS_Terrain(VertexInput_Terrain input)
{
    VertexOutput_Terrain output;
    output.Position = input.Position;
	// sample로 해도 되는데 mipmap 잇으면 고려하려고 sample level 쓴거
    output.Position.y = HeightMap.SampleLevel(HeightMapSampler, input.Uv, 0).r;

    output.Uv = input.Uv;
    output.BoundsY = input.BoundsY;

    return output;
}

// Billboard

struct VertexInput_Billboard
{
    float4 Center : POSITION0;

    matrix World : INSTANCE0;
    int TextureId : INSTANCE4;
	
    uint InstanceId : SV_INSTANCEID0;
};

struct VertexOutput_Billboard
{
    float4 Center : POSITION0;

    matrix World : MATRIX0;
    int TextureId : TEXTURE0;

    uint InstanceId : UINT0;
};

VertexOutput_Billboard VS_Billboard(VertexInput_Billboard input)
{
    VertexOutput_Billboard output;

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

struct GeometryOutput_Billboard
{
    float4 Position : SV_POSITION0;

    //float3 Normal : NORMAL0;
    //float3 Tangent : TANGENT0;

    float2 Uv : UV0;
    float2 PixelUv : Uv1;

    int TextureId : TEXTURE0;

    uint InstanceId : UINT0;
};

float2 TexCoord[4] =
{
    float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0)
};

[maxvertexcount(4)]
void GS_Billboard(point VertexOutput_Billboard input[1], inout TriangleStream<GeometryOutput_Billboard> stream)
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

	//// 정규 직교
 //   float3 e0 = v[1] - v[0];
 //   float3 e1 = v[2] - v[0];

 //   float u0 = TexCoord[1].x - TexCoord[0].x;
 //   float u1 = TexCoord[2].x - TexCoord[0].x;
 //   float v0 = TexCoord[1].y - TexCoord[0].y;
 //   float v1 = TexCoord[2].y - TexCoord[0].y;
 //   float r = 1.0f / (u0 * v1 - v0 * u1);

	//// tangent 벡터
 //   float3 tangent;
 //   tangent.x = r * (v1 * e0.x - v0 * e1.x);
 //   tangent.y = r * (v1 * e0.y - v0 * e1.y);
 //   tangent.z = r * (v1 * e0.z - v0 * e1.z);

    GeometryOutput_Billboard output;
	[unroll]
    for (int i = 0; i < 4; i++)
    {
        //output.Position = mul(v[i], input[0].World);
        //output.Position = mul(output.Position, View);

        output.Position = mul(v[i], View);
        output.Position = mul(output.Position, Projection);

        //output.Normal = look;

        //output.Tangent = normalize(tangent);

        output.Uv = TexCoord[i];

        output.PixelUv = float2((v[i].x + 512) / Width, (v[i].z + 512) / Height);;

        output.InstanceId = input[0].InstanceId;

        output.TextureId = input[0].TextureId;

        stream.Append(output);
    }
}

//-----------------------------------------------------------------------------
// Hull Shader
//-----------------------------------------------------------------------------

float CalcTessFactor(float3 position)
{
    float d = distance(position, ViewPosition);

    float s = saturate((d - MinDistance) / (MaxDistance - MinDistance));

    return pow(2, lerp(MaxTessellation, MinTessellation, s));
}

bool AabbBehindPlaneTest(float3 center, float3 extents, float4 plane)
{
    float3 n = abs(plane.xyz);
	
    float r = dot(extents, n);

    float s = dot(float4(center, 1.0f), plane);

    return (s + r) < 0.0f;
}

bool AabbOutsideFrustumTest(float3 center, float3 extents)
{
	[unroll]
    for (int i = 0; i < 6; i++)
    {
		[flatten]
        if (AabbBehindPlaneTest(center, extents, WorldFrustumPlanes[i]))
        {
            return true;
        }
    }

    return false;
}

struct ConstantOutput_Terrain
{
    float Edges[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

ConstantOutput_Terrain HS_Constant_Terrain(InputPatch<VertexOutput_Terrain, 4> input, uint patchID : SV_PrimitiveId)
{
    ConstantOutput_Terrain output;

    float minY = input[0].BoundsY.x;
    float maxY = input[0].BoundsY.y;

    float3 vMin = float3(input[2].Position.x, minY, input[2].Position.z);
    float3 vMax = float3(input[1].Position.x, maxY, input[1].Position.z);

    float3 boxCenter = 0.5f * (vMin + vMax);
    float3 boxExtents = 0.5f * (vMax - vMin);

	[flatten]
    if (AabbOutsideFrustumTest(boxCenter, boxExtents))
    {
        output.Edges[0] = 0.0f;
        output.Edges[1] = 0.0f;
        output.Edges[2] = 0.0f;
        output.Edges[3] = 0.0f;

        output.Inside[0] = 0.0f;
        output.Inside[1] = 0.0f;

        return output;
    }

    float3 e0 = (input[0].Position + input[2].Position).xyz * 0.5f;
    float3 e1 = (input[0].Position + input[1].Position).xyz * 0.5f;
    float3 e2 = (input[1].Position + input[3].Position).xyz * 0.5f;
    float3 e3 = (input[2].Position + input[3].Position).xyz * 0.5f;

    float3 c = 0.25f *
		(input[0].Position + input[1].Position + input[2].Position + input[3].Position).xyz;

    output.Edges[0] = CalcTessFactor(e0);
    output.Edges[1] = CalcTessFactor(e1);
    output.Edges[2] = CalcTessFactor(e2);
    output.Edges[3] = CalcTessFactor(e3);

    output.Inside[0] = CalcTessFactor(c);
    output.Inside[1] = output.Inside[0];

    return output;
}

struct HullOutput_Terrain
{
    float4 Position : Position0;
    float2 Uv : Uv0;
};

[domain("quad")] // 삼각형 용도로 쓰겠다는거
// line, line-adj, tri, tri-adj 등등 있음 HS 라고 치면 ms 메뉴얼 나옴 // adj 는 인접 점 4개 들어가면 띄엄띄엄 들어감 점선처럼
[partitioning("fractional_even")] // 잘라내는 기준이 int형만 들어갈 수 있음 float이면 무시됨 float 쓰면 정밀해지는데 속도는 느려짐
//[partitioning("fractional_odd")] // 잘라내는 기준이 int형만 들어갈 수 있음 float이면 무시됨 float 쓰면 정밀해지는데 속도는 느려짐
// 대규모 지형에서 이동시 팝핑현상 나타는 이유가 이 때문임
[outputtopology("triangle_cw")] // 정점을 어떤 방향으로 감을껀지 항상 시계방향으로 써왔음
[outputcontrolpoints(4)] // 컨트롤 포인터 몇개 단위로 보낼지
[patchconstantfunc("HS_Constant_Terrain")] // 사용 할 상수 hull shader 이름이 뭔지

// 쓰면 속도가 올라간다고 함 안써도 되는데 미리 64개의 공간을 만들어 놓은다고 함 아니면 들어오는 순서에 누적되서 된다고함
[maxtessfactor(64.0f)] // 내가 써줄 최대 tess factor 최대 32 * 2 까지 가능 최적화 맞추기 위해 64개로 맞춘다고 함

HullOutput_Terrain HS_Terrain(InputPatch<VertexOutput_Terrain, 4> input, uint pointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HullOutput_Terrain output;
    output.Position = input[pointID].Position;
    output.Uv = input[pointID].Uv;

    return output;
}

//-----------------------------------------------------------------------------
// Domain Shader
//-----------------------------------------------------------------------------

struct DomainOutput_Terrain
{
    float4 Position : SV_Position0;
    float3 wPosition : Position1;
    float2 Uv : Uv0;
    float2 TileUv : Uv1;

    float2 PixelUv : Uv2;
    float PixelHeight : Height1;
};

[domain("quad")]
// 사각형은 제어점이 2개라 float2 uv로 받아도 됨 삼각형은 float3로 받아야함
DomainOutput_Terrain DS_Terrain(ConstantOutput_Terrain input, float2 uvw : SV_DomainLocation, const OutputPatch<HullOutput_Terrain, 4> patch)
{
    DomainOutput_Terrain output;

    float3 p0 = lerp(patch[0].Position, patch[1].Position, uvw.x).xyz;
    float3 p1 = lerp(patch[2].Position, patch[3].Position, uvw.x).xyz;
    float3 position = lerp(p0, p1, uvw.y);
    output.wPosition = position;

    float2 uv0 = lerp(patch[0].Uv, patch[1].Uv, uvw.x);
    float2 uv1 = lerp(patch[2].Uv, patch[3].Uv, uvw.x);
    output.Uv = lerp(uv0, uv1, uvw.y);

    output.TileUv = output.Uv * TexScale;

    output.wPosition.y = HeightMap.SampleLevel(HeightMapSampler, output.Uv, 0).r;

    output.Position = mul(float4(output.wPosition, 1), World);
    output.Position = mul(float4(output.wPosition, 1), View);
    output.Position = mul(output.Position, Projection);

    output.PixelUv = float2((output.wPosition.x + 512) / Width, (output.wPosition.z + 512) / Height);
    output.PixelHeight = output.wPosition.y / MaxHeight;

    return output;
}

//-----------------------------------------------------------------------------
// Pisxel Shader
//-----------------------------------------------------------------------------

Texture2DArray LayerMapArray;
Texture2D BlendMap;

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;

    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PS_Terrain(DomainOutput_Terrain input) : SV_TARGET
{
    float4 color = float4(input.PixelUv, 0, 1);

    return color;
}

Texture2DArray BillboardTextures;
SamplerState Sampler;

float4 PS_Billboard(GeometryOutput_Billboard input) : SV_TARGET
{
    float3 uvw = float3(input.Uv, input.TextureId);

    float4 color = BillboardTextures.Sample(Sampler, uvw);

    float alpha = color.a;

    color.a = alpha;

    clip(color.a - 0.5f);

    //return color;

    return float4(input.PixelUv, 1, 1);
}

//-----------------------------------------------------------------------------
// Techinques
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Terrain()));
        SetHullShader(CompileShader(hs_5_0, HS_Terrain()));
        SetDomainShader(CompileShader(ds_5_0, DS_Terrain()));
        SetPixelShader(CompileShader(ps_5_0, PS_Terrain()));
    }
}

technique11 T1
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Billboard()));
        SetGeometryShader(CompileShader(gs_5_0, GS_Billboard()));
        SetPixelShader(CompileShader(ps_5_0, PS_Billboard()));
    }
}
