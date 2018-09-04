#include "000_Header.hlsl"

cbuffer PS_Line : register(b10)
{
    int LineType;
    float3 LineColor; 

    int Spacing; // 한칸의 넓이
    float Thickness; // 선의 두께
}


float3 Line(float3 oPosition)
{
    [branch] // branch 쓰면 else if 쓸 수 있음
    // 이거말고 flatten도 있음
    // 쉐이더에서는 if else쪽 둘다 실행 후 조건에 맞는거만 씀
    // flatten 둘 다 실행 후 한 가지 선택 else if 전개
    // branch은 우리가 아는 c방식으로 함
    if (LineType == 1)
    {
        float2 grid = float2(0, 0);
        grid.x = frac(oPosition.x / (float) Spacing); // spacing 으로 나눈 건 0~1 사이로 만들려고
        grid.y = frac(oPosition.z / (float) Spacing); // spacing 으로 나눈 건 0~1 사이로 만들려고

        [flatten]
        if(grid.x < Thickness || grid.y < Thickness)
            return LineColor;
    }
    else if (LineType == 2)
    {
        float2 grid = oPosition.xz / (float) Spacing;

        // 0.5 뺀거는 시작지점 맞추려고 하는거 (0.5f 밴거 왼쪽 끝)
        float2 range = abs(frac(grid - 0.5f) - 0.5f);
        float2 speed = fwidth(grid); // 편미분, 선의 간격
        // = abs(ddx(x) + ddy(y))

        // 선의 두깨를 만들어주려고 하는거
        float2 pixel = range / speed;
        // 이부분은 고민해보자
        float weights = saturate(min(pixel.x, pixel.y) - Thickness);

        // 선색이랑 원래 가지고 있는 색이랑 보간
        return lerp(LineColor, float3(0, 0, 0), weights);
    }

    return float3(0, 0, 0);
}

struct PixelInput
{
    float4 Position : SV_POSITION; // SV만 정해진 예약어 나머진 편한대로 쓰면 됨
    float3 oPosition : POSITION0;
    float2 Uv : UV0;
    float3 Normal : NORMAL0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.oPosition = input.Position;

    // 사실 월드 변환된 이후에 position에 따라 해야되긴 함
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);

    output.Uv = input.Uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = 0;

    float4 diffuse = DiffuseMap.Sample(DiffuseSampler, input.Uv);

    DiffuseLighting(color, diffuse, input.Normal);

    // Line Drawing
    color = color + float4(Line(input.oPosition), 0);

    return color;
    // 디버깅 하기 위해서 normal을 반환형인 색상값으로
    //return float4((input.Normal * 0.5f) + 0.5f, 1);
}