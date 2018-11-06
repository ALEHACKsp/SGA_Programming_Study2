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

// c0 c1으로 하면 끊어주겠다는거 1byte는 버리고 안쓰게됨
cbuffer PS_Ambient : register(b10)
{
    float4 DirectionColor;

	// 디자이너들이 조정하는 값
	// 이렇게 하면 3바이트만 잡힘 근데 미리 처리해두셔서 이렇게 안쓸꺼
    //float3 AmbientFloor : packoffset(c0); // 바닥색
    //float3 AmbientCeil : packoffset(c1); // 하늘에서 들어오는 빛감

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
    float up = normal.y * 0.5f + 0.5f; // 0 ~ 1로 떨어뜨리려고
    float3 ambient = AmbientFloor + up * AmbientCeil; // 선형보간해도 되지만 이게 더 빠름 정밀도 차이도 없고

    return ambient * color;
}

float3 CalcDirectional(float3 wPosition, float3 cPosition, Material material)
{
    float d = dot(-Direction, material.Normal);
    float3 color = DirectionColor.rgb * saturate(d);

    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye - Direction);
    float h = saturate(dot(halfWay, material.Normal)); // R E의 사이각이랑 유사값 나옴
	// 이게 정석 전에껀 간소화한거
    color += DirectionColor.rgb * pow(h, material.SpecularExp) * material.SpecularIntensity; 

    return color * material.DiffuseColor.rgb; // 다시 한번 곱해서 선형색 구한거
}

float3 CalcPoint(float3 wPosition, float3 cPosition, Material material)
{
	// PointLight
    float3 toLight = PointLightPosition.xyz - wPosition;
    float distanceToLight = length(toLight);
    toLight /= distanceToLight; // 정규화 normalize

	// Diffuse
    float d = dot(toLight, material.Normal);
    float3 color = PointLightColor.rgb * saturate(d);

	// Specular
    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye + toLight);
    float h = saturate(dot(halfWay, material.Normal)); // R E의 사이각이랑 유사값 나옴
	// 이게 정석 전에껀 간소화한거
    color += PointLightColor.rgb * pow(h, material.SpecularExp) * material.SpecularIntensity;

	// 감쇄 구형으로 감쇄되는거
    float distanceToLightNormal = 1.0f - saturate(distanceToLight * 1 / PointLightRange);
    float attention = distanceToLightNormal * distanceToLightNormal; // 선형 공간
    color *= material.DiffuseColor * attention;

    return color;
}

float3 CalcSpot(float3 wPosition, float3 cPosition, Material material)
{
	// SpotLight
    float3 toLight = SpotLightPosition.xyz - wPosition;
    float distanceToLight = length(toLight);
    toLight /= distanceToLight; // 정규화 normalize

	// Diffuse
    float d = dot(toLight, material.Normal);
    float3 color = SpotLightColor.rgb * saturate(d);

	// Specular
    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye + toLight);
    float h = saturate(dot(halfWay, material.Normal)); // R E의 사이각이랑 유사값 나옴
	// 이게 정석 전에껀 간소화한거
    color += SpotLightColor.rgb * pow(h, material.SpecularExp) * material.SpecularIntensity;

	// 감쇄 Attenuation 콘형으로 감쇄된다는거 
    float cosAngle = dot(-SpotLightDirToLight, toLight); // 조명 빛을 받을 강도

    float outer = cos(radians(SpotLightOuter));
    float inner = 1.0f / cos(radians(SpotLightInner));

    //float coneAttention = saturate((cosAngle - outer));
    float coneAttention = saturate((cosAngle - outer) * inner);
    coneAttention *= coneAttention;

    float distanceToLightNormal = 1.0f - saturate(distanceToLight * 1 / SpotLightRange);
    float attention = distanceToLightNormal * distanceToLightNormal; // 선형 공간
    color *= material.DiffuseColor * attention * coneAttention;

    //return float3(1, 1, 1);
    //return float3(coneAttention, coneAttention, coneAttention);
    return color;
}

float3 CalcCapsule(float3 wPosition, float3 cPosition, Material material)
{
	// CapsuleLightDirection 이 B 지점을 가리키는 방향

	// CapsuleLight
    float3 start = wPosition - CapsuleLightPosition;
    float3 nor = normalize(CapsuleLightDirection);
    float distanceOnLine = dot(start, nor) / CapsuleLightLength; // dot 하면 투영시켜서 length가 나옴
    distanceOnLine = saturate(distanceOnLine) * CapsuleLightLength; // saturate 한거는 구 부분 빼주는거
	// distanceOnLine : 현재 빛에서 픽셀로 수직으로 온 위치

	// 여기까지 PointLight 픽셀로 부터 수직인 위치 찾는거라고 보면됨
	
	// 여기부터는 PointLight
    float3 pointOnLine = CapsuleLightPosition + nor * distanceOnLine; // 가장 가까운 점
    float3 toLight = pointOnLine - wPosition;
    float distanceToLight = length(toLight);

	// 여기까지 현재 픽셀과의 최단거리 - 점과 선분의 최단거리는 항상 수직이어야함

	// Diffuse
    toLight /= distanceToLight;
    float d = dot(toLight, material.Normal);
    float3 color = material.DiffuseColor.rgb * saturate(d);

	// Specular
    float3 toEye = normalize(cPosition - wPosition);
    float3 halfWay = normalize(toEye + toLight);
    float h = saturate(dot(halfWay, material.Normal)); // R E의 사이각이랑 유사값 나옴
	// 이게 정석 전에껀 간소화한거
    color += pow(h, material.SpecularExp) * material.SpecularIntensity;

	// 감쇄 Attenuation 콘형으로 감쇄된다는거 
    float distanceToLightNormal = 1.0f - saturate(distanceToLight / CapsuleLightRange);
    float attention = distanceToLightNormal * distanceToLightNormal; // 선형 공간
    color *= CapsuleLightColor * attention;

    return color;
}

Material CreateMaterial(float3 normal, float2 uv)
{
    Material material;
    material.Normal = normalize(normal);
    //material.DiffuseColor = DiffuseMap.Sample(DiffuseSampler, uv);
    material.DiffuseColor = Diffuse;
    material.DiffuseColor.rgb *= material.DiffuseColor.rgb; // 선형색 공간으로 바꾸는거

	// 블링 퐁 모델 
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
//    float4 color = float4(Diffuse.rgb * Diffuse.rgb, Diffuse.a); // 여기서 a 감도
 
//    color.rgb =  CalcAmbient(normal, color.rgb);
//    color.a = 1.0f;

//    return color;
//}