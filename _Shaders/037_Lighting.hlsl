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

// Directional
float4 PS(PixelInput input) : SV_TARGET
{
    Material material = CreateMaterial(input.Normal, input.Uv);

    float4 color = 0;

    color.rgb = CalcAmbient(material.Normal, material.DiffuseColor.rgb);
    color.rgb *= CalcDirectional(input.wPosition.xyz, input.cPosition, material);
    color.a = 1.0f;

    return color;
}

// Ambient
//float4 PS(PixelInput input) : SV_TARGET
//{
//    float3 normal = normalize(input.Normal);
//    float4 color = float4(Diffuse.rgb * Diffuse.rgb, Diffuse.a); // 여기서 a 감도
 
//    color.rgb =  CalcAmbient(normal, color.rgb);
//    color.a = 1.0f;

//    return color;
//}