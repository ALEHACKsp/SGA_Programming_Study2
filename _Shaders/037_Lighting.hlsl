#include "000_Header.hlsl"

struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL0;
    float2 Uv : UV0;
    float4 wPosition : POSITION0;
    float3 cPosition : POSTIION1;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    matrix world = BoneWorld();
    output.Position = mul(input.Position, world);
    output.wPosition = output.Position;

    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = WorldNormal(input.Normal, world);

    output.Uv = input.Uv;
    output.cPosition = CameraPosition();

    return output;
}

// c0 c1���� �ϸ� �����ְڴٴ°� 1byte�� ������ �Ⱦ��Ե�
cbuffer PS_Ambient : register(b10)
{
    float4 DirectionColor;

	// �����̳ʵ��� �����ϴ� ��
	// �̷��� �ϸ� 3����Ʈ�� ���� �ٵ� �̸� ó���صμż� �̷��� �Ⱦ���
    //float3 AmbientFloor : packoffset(c0); // �ٴڻ�
    //float3 AmbientCeil : packoffset(c1); // �ϴÿ��� ������ ����

    float3 AmbientFloor;
    float PS_Ambient_Padding;

    float3 AmbientCeil;
}

cbuffer PS_Specular : register(b11)
{
    float SpecularExp;
    float SpecularIntentisty;
}

cbuffer PS_Point : register(b12)
{
    float3 PointLightPosition;
    float PointLightRange;

    float4 PointLightColor;
}

cbuffer PS_Spot : register(b13)
{
    float4 SpotLightColor;

    float3 SpotLightPosition;
    float SpotLightRange;

    float3 SpotLightDirToLight;
    float SpotLightOuter;

    float SpotLightInner;
    float3 Padding;
}

cbuffer PS_Capsule : register(b9)
{
    float4 CapsuleLightColor;

    float3 CapsuleLightPosition;
    float CapsuleLightRange;

    float3 CapsuleLightDirection;
    float CapsuleLightLength;
}

struct Material
{
    float3 Normal;
    float4 DiffuseColor;
    float SpecularExp;
    float SpecularIntensity;
};

float3 CalcAmbient(float3 normal, float3 color)
{
    float up = normal.y * 0.5f + 0.5f; // 0 ~ 1�� ����߸�����
    float3 ambient = AmbientFloor + up * AmbientCeil; // ���������ص� ������ �̰� �� ���� ���е� ���̵� ����

    return ambient * color;
}

float3 CalcDirectional(float3 wPosition, float3 cPosition, Material material)
{
    float d = dot(-Direction, material.Normal);
    float3 color = DirectionColor.rgb * saturate(d);

    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye - Direction);
    float h = saturate(dot(halfWay, material.Normal)); // R E�� ���̰��̶� ���簪 ����
	// �̰� ���� ������ ����ȭ�Ѱ�
    color += DirectionColor.rgb * pow(h, material.SpecularExp) * material.SpecularIntensity; 

    return color * material.DiffuseColor.rgb; // �ٽ� �ѹ� ���ؼ� ������ ���Ѱ�
}

float3 CalcPoint(float3 wPosition, float3 cPosition, Material material)
{
	// PointLight
    float3 toLight = PointLightPosition.xyz - wPosition;
    float distanceToLight = length(toLight);
    toLight /= distanceToLight; // ����ȭ normalize

	// Diffuse
    float d = dot(toLight, material.Normal);
    float3 color = PointLightColor.rgb * saturate(d);

	// Specular
    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye + toLight);
    float h = saturate(dot(halfWay, material.Normal)); // R E�� ���̰��̶� ���簪 ����
	// �̰� ���� ������ ����ȭ�Ѱ�
    color += PointLightColor.rgb * pow(h, material.SpecularExp) * material.SpecularIntensity;

	// ���� �������� ����Ǵ°�
    float distanceToLightNormal = 1.0f - saturate(distanceToLight * 1 / PointLightRange);
    float attention = distanceToLightNormal * distanceToLightNormal; // ���� ����
    color *= material.DiffuseColor * attention;

    return color;
}

float3 CalcSpot(float3 wPosition, float3 cPosition, Material material)
{
	// SpotLight
    float3 toLight = SpotLightPosition.xyz - wPosition;
    float distanceToLight = length(toLight);
    toLight /= distanceToLight; // ����ȭ normalize

	// Diffuse
    float d = dot(toLight, material.Normal);
    float3 color = SpotLightColor.rgb * saturate(d);

	// Specular
    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye + toLight);
    float h = saturate(dot(halfWay, material.Normal)); // R E�� ���̰��̶� ���簪 ����
	// �̰� ���� ������ ����ȭ�Ѱ�
    color += SpotLightColor.rgb * pow(h, material.SpecularExp) * material.SpecularIntensity;

	// ���� Attenuation �������� ����ȴٴ°� 
    float cosAngle = dot(-SpotLightDirToLight, toLight); // ���� ���� ���� ����

    float outer = cos(radians(SpotLightOuter));
    float inner = 1.0f / cos(radians(SpotLightInner));

    //float coneAttention = saturate((cosAngle - outer));
    float coneAttention = saturate((cosAngle - outer) * inner);
    coneAttention *= coneAttention;

    float distanceToLightNormal = 1.0f - saturate(distanceToLight * 1 / SpotLightRange);
    float attention = distanceToLightNormal * distanceToLightNormal; // ���� ����
    color *= material.DiffuseColor * attention * coneAttention;

    //return float3(1, 1, 1);
    //return float3(coneAttention, coneAttention, coneAttention);
    return color;
}

float3 CalcCapsule(float3 wPosition, float3 cPosition, Material material)
{
	// CapsuleLightDirection �� B ������ ����Ű�� ����

	// CapsuleLight
    float3 start = wPosition - CapsuleLightPosition;
    float3 nor = normalize(CapsuleLightDirection);
    float distanceOnLine = dot(start, nor) / CapsuleLightLength; // dot �ϸ� �������Ѽ� length�� ����
    distanceOnLine = saturate(distanceOnLine) * CapsuleLightLength; // saturate �ѰŴ� �� �κ� ���ִ°�
	// distanceOnLine : ���� ������ �ȼ��� �������� �� ��ġ

	// ������� PointLight �ȼ��� ���� ������ ��ġ ã�°Ŷ�� �����
	
	// ������ʹ� PointLight
    float3 pointOnLine = CapsuleLightPosition + nor * distanceOnLine; // ���� ����� ��
    float3 toLight = pointOnLine - wPosition;
    float distanceToLight = length(toLight);

	// ������� ���� �ȼ����� �ִܰŸ� - ���� ������ �ִܰŸ��� �׻� �����̾����

	// Diffuse
    toLight /= distanceToLight;
    float d = dot(toLight, material.Normal);
    float3 color = material.DiffuseColor.rgb * saturate(d);

	// Specular
    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye + toLight);
    float h = saturate(dot(halfWay, material.Normal)); // R E�� ���̰��̶� ���簪 ����
	// �̰� ���� ������ ����ȭ�Ѱ�
    color += pow(h, material.SpecularExp) * material.SpecularIntensity;

	// ���� Attenuation �������� ����ȴٴ°� 
    float distanceToLightNormal = 1.0f - saturate(distanceToLight / CapsuleLightRange);
    float attention = distanceToLightNormal * distanceToLightNormal; // ���� ����
    color *= CapsuleLightColor * attention;

    return color;
}

Material CreateMaterial(float3 normal, float2 uv)
{
    Material material;
    material.Normal = normalize(normal);
    //material.DiffuseColor = DiffuseMap.Sample(DiffuseSampler, uv);
    material.DiffuseColor = Diffuse;
    material.DiffuseColor.rgb *= material.DiffuseColor.rgb; // ������ �������� �ٲٴ°�

	// �� �� �� 
    material.SpecularExp = SpecularExp;
    material.SpecularIntensity = SpecularIntentisty;

    return material;
}

//// CapsuleLight
//float4 PS(PixelInput input) : SV_TARGET
//{
//    Material material = CreateMaterial(input.Normal, input.Uv);

//    float4 color = 1;

//    color.rgb *= CalcCapsule(input.wPosition.xyz, input.cPosition, material);
//    color.a = 1.0f;

//    return color;
//}

// SpotLight
float4 PS(PixelInput input) : SV_TARGET
{
    Material material = CreateMaterial(input.Normal, input.Uv);

    float4 color = 1;

    color.rgb *= CalcSpot(input.wPosition.xyz, input.cPosition, material);
    color.a = 1.0f;
	
    //return float4(1, 1, 1, 1);
    return color;
}

//// PointLight
//float4 PS(PixelInput input) : SV_TARGET
//{
//    Material material = CreateMaterial(input.Normal, input.Uv);

//    float4 color = 1;

//    color.rgb *= CalcPoint(input.wPosition.xyz, input.cPosition, material);
//    color.a = 1.0f;

//    return color;
//}

// Directional
//float4 PS(PixelInput input) : SV_TARGET
//{
//    Material material = CreateMaterial(input.Normal, input.Uv);

//    float4 color = 0;

//    color.rgb = CalcAmbient(material.Normal, material.DiffuseColor.rgb);
//    color.rgb *= CalcDirectional(input.wPosition.xyz, input.cPosition, material);
//    color.a = 1.0f;

//    return color;
//}

// Ambient
//float4 PS(PixelInput input) : SV_TARGET
//{
//    float3 normal = normalize(input.Normal);
//    float4 color = float4(Diffuse.rgb * Diffuse.rgb, Diffuse.a); // ���⼭ a ����
 
//    color.rgb =  CalcAmbient(normal, color.rgb);
//    color.a = 1.0f;

//    return color;
//}