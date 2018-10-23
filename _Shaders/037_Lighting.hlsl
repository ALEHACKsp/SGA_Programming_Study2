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
//    float4 color = float4(Diffuse.rgb * Diffuse.rgb, Diffuse.a); // ���⼭ a ����
 
//    color.rgb =  CalcAmbient(normal, color.rgb);
//    color.a = 1.0f;

//    return color;
//}