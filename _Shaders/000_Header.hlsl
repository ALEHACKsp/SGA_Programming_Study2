cbuffer VS_ViewProjection : register(b0) // register buffer 0�� ���ڴٴ°�
{
    matrix View;
    matrix Projection;
    matrix ViewInverse;
}

cbuffer VS_World : register(b1)
{
    matrix World;
}

cbuffer VS_Bones : register(b2)
{
    matrix Bones[128];
    matrix BoneScale;
}

cbuffer VS_BoneIndex : register(b3)
{
    int BoneIndex;
}

cbuffer PS_Light : register(b0) // ���ؽ� ���̴��� �����̹Ƿ� �ٽ� 0������
{
    float3 Direction;
}

// material ���� �ǹ�
cbuffer PS_Material : register(b1)
{
    float4 Diffuse; // ���İ��� ��� Diffuse alpha�� Shininess ���⵵��
    float4 Specular;
    //float4 Detail;

    float Shininess; // specular power��� ���⵵ ��
    //float Brightness;
}

Texture2D DiffuseMap : register(t0);
SamplerState DiffuseSampler : register(s0);

Texture2D SpecularMap : register(t1);
SamplerState SpecularSampler : register(s1);

Texture2D NormalMap : register(t2);
SamplerState NormalSampler : register(s2);

Texture2D DetailMap : register(t3);
SamplerState DetailSampler : register(s3);

struct VertexColor
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
};

struct VertexColorNormal
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
    float3 Normal : NORMAL0;
};

struct VertexTexture
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
};

struct VertexTextureNormal
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
};

struct VertexTextureNormalTangent
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
};

struct VertexColorTextureNormal
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
};

struct VertexTextureNormalTangentBlend
{
    float4 Position : POSITION0;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL0;
    float3 Tangent : TANGENT0;
    float4 BlendIndices : BLENDINDICES0;
    float4 BlendWeights : BLENDWEIGHTS0;
};

///////////////////////////////////////////////////////////////////////////////

matrix BoneWorld()
{
    //return mul(Bones[BoneIndex], BoneScale);
    return Bones[BoneIndex];
}

matrix SkinWorld(float4 blendIndices, float4 blendWeights)
{
    float4x4 transform = 0;
    transform += mul(blendWeights.x, Bones[(uint) blendIndices.x]);
    transform += mul(blendWeights.y, Bones[(uint) blendIndices.y]);
    transform += mul(blendWeights.z, Bones[(uint) blendIndices.z]);
    transform += mul(blendWeights.w, Bones[(uint) blendIndices.w]);
    
	//transform = mul(transform, BoneScale);
    return transform;
}

float3 CameraPosition()
{
    return ViewInverse._41_42_43;
}

float3 WorldViewDirection(float4 wPosition)
{
    return normalize(CameraPosition() - wPosition.xyz);
}

float3 WorldNormal(float3 normal)
{
    normal = normalize(mul(normal, (float3x3) World));

    return normal;
}

float3 WorldNormal(float3 normal, matrix world)
{
    normal = normalize(mul(normal, (float3x3) world));

    return normal;
}

float3 WorldTangent(float3 tangent, matrix world)
{
    tangent = normalize(mul(tangent, (float3x3) world));

    return tangent;
}

///////////////////////////////////////////////////////////////////////////////


// ������ �� ��
void DiffuseLighting(inout float4 color, float3 normal)
{
    //float3 light = _direction * -1;
    // ���� ����
    // saturate 0 ~ 1���� �������ִ� �Լ�
    float intensity = saturate(dot(normal, -Direction));

    color = color + Diffuse * intensity;
}

void DiffuseLighting(inout float4 color, float4 diffuse, float3 normal)
{
    //float3 light = _direction * -1;
    // ���� ����
    // saturate 0 ~ 1���� �������ִ� �Լ�
    float intensity = saturate(dot(normal, -Direction));

    color = color + Diffuse * diffuse * intensity;
}

// viewDirection �������κ��� ī�޶� ����
void SpecularLighting(inout float4 color, float3 normal, float3 viewDirection)
{
    float3 reflection = reflect(Direction, normal);
    float intensity = saturate(dot(reflection, viewDirection));
    float specular = pow(intensity, Shininess);

    color = color + Specular * specular;
}

void SpecularLighting(inout float4 color, float4 specularMap, float3 normal, float3 viewDirection)
{
    float3 reflection = reflect(Direction, normal);
    float intensity = saturate(dot(reflection, viewDirection));
    float specular = pow(intensity, Shininess);

    color = color + Specular * specular * specularMap;
}

void NormalMapping(inout float4 color, float4 normalMap, float3 normal, float3 tangent)
{
    float3 N = normal; // Z���̶� ���ε�
    float3 T = normalize(tangent - dot(tangent, N) * N); // X �� ���� �׶�����Ʈ ��
    float3 B = cross(T, N); // Y

    float3x3 TBN = float3x3(T, B, N);

	// rgb 0~1 �������� ����°�
    float3 coord = 2.0f * normalMap - 1.0f;
    float3 bump = mul(coord, TBN); // max���� normal mapping�� bump mapping�̶� �θ�

    float intensity = saturate(dot(bump, -Direction));
    color = color * intensity;
}

///////////////////////////////////////////////////////////////////////////////

struct PointLight
{
    float3 Position;
    float Range;
    float3 Color;
    float Intensity;
};

cbuffer PS_PointLights : register(b2)
{
    PointLight PointLights[32];

    int PointLightCount;
}

void PointLighting(inout float4 color, PointLight light, float4 wPosition, float3 normal)
{
    float dist = distance(light.Position, wPosition.xyz);
    float intensity = saturate((light.Range - dist) / light.Range);

    intensity = pow(intensity, light.Intensity); // ������ ����

    color = color + float4(light.Color, 0) * intensity;
}

///////////////////////////////////////////////////////////////////////////////

struct SpotLight
{
    float3 Position;
    float InnerAngle;
    float3 Color;
    float OuterAngle;
    float3 Direction;

    float SpotLight_Padding;
};

cbuffer PS_SpotLights : register(b3)
{
    SpotLight SpotLights[32];

    int SpotLightCount;
}

void SpotLighting(inout float4 color, SpotLight light, float4 wPosition, float3 normal)
{
    float3 lightDir = normalize(light.Position - wPosition.xyz);

    float intensity = 0;
    float lightAngle = dot(-light.Direction, lightDir);
    if (lightAngle > 0.0f)
		// lerp �� �����Ѱ� 0~1���� ����� �������ε� hermit �������� �ϴ°�
		// OuterAngle ~> InnerAngle �� ������ ����� ������ �������
        intensity = smoothstep(light.InnerAngle, light.OuterAngle, lightAngle);
	
    color = color + float4(light.Color, 0) * intensity;
}

///////////////////////////////////////////////////////////////////////////////

struct AreaLight
{
    float3 Position;
    float Light_Padding;

    float3 Color;
    float Light_Padding2;

    float3 HalfSize;
    float Light_Padding3;
};

cbuffer PS_AreaLights : register(b4)
{
    AreaLight AreaLights[32];

    int AreaLightCount;
}

void AreaLighting(inout float4 color, AreaLight light, float4 wPosition, float3 normal)
{

    float intensity = 0;
    float dist = length(light.Position - wPosition.xyz);
    float dist2 = length(light.Position - light.HalfSize);
   
    if (//(dist <= dist2) &&
		(wPosition.x >= light.Position.x - light.HalfSize.x) &&
		(wPosition.x <= light.Position.x + light.HalfSize.x) &&
		(wPosition.z >= light.Position.z - light.HalfSize.z) &&
		(wPosition.z <= light.Position.z + light.HalfSize.z))
    {
        intensity = smoothstep(light.Position.y - light.HalfSize.y, light.Position.y, wPosition.y);
        //intensity = 1.0f - intensity;
    }
	//else
 //   {
 //       float3 dir = normalize(light.Position - wPosition.xyz);
		
 //       float angle = dot(dir, float3(0, 1, 0));
 //       intensity = smoothstep(0, 1, angle);
 //       //intensity = 1.0f - intensity;
 //   }

    color = color + float4(light.Color, 0) * intensity;
}