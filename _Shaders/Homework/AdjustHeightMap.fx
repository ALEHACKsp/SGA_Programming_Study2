//-----------------------------------------------------------------------------
// Compute Shader
//-----------------------------------------------------------------------------

static const int N = 8; // 그룹 스레드 크기

Texture2D Input;
RWTexture2D<float4> Output;

cbuffer CB_Brush
{
    int Type;
    float3 Location;

    int Range;
    float3 Color;

    float Capacity = 0.1f;
};

float pi = 3.14159265f;

[numthreads(N, N, 1)]
void Brush(int3 groupThreadId : SV_GroupThreadId, int3 dispatchThreadId : SV_DispatchThreadId)
{
    int width, height;
    Input.GetDimensions(width, height);

    int posX = int(Location.x) * 2;
    int posY = height - int(Location.z) * 2;

    int size = Range * 2;

    if (posX == dispatchThreadId.x && posY == dispatchThreadId.y)
    {
        int left, right, top, bottom;
        left = max(posX - size, 0);
        right = min(posX + size, width);
        bottom = max(posY - size, 0);
        top = min(posY + size, height);

        for (int x = left; x <= right; x++)
        {
            for (int y = bottom; y <= top; y++)
            {
                float2 pos = float2(x / 2, (height - y) / 2);
                float temp = Capacity;

				// Check Circle
                if (Type == 2 || Type == 3)
                {
                    float dx = pos.x - Location.x;
                    float dz = pos.y - Location.z;

                    float dist = sqrt(dx * dx + dz * dz);
                    if (dist > Range)
                        continue;
                }

				// Sin
                if (Type == 3)
                {
                    float dx = pos.x - Location.x;
                    float dz = pos.y - Location.z;

                    float dist = sqrt(dx * dx + dz * dz);
                    dist = (size - dist) / (float) size * (pi / 2.0f);
                    temp *= sin(dist);
                }

				// Flat
                if (Type == 4)
                {
                    int cnt = 0;
                    float avg = 0;

                    for (int xp = x - 1; xp <= x + 1; xp++)
                    {
                        for (int yp = y - 1; yp <= y + 1; yp++)
                        {
                            if (xp < 0 || xp > width || yp < 0 || yp > height) 
                                continue;

                            cnt += 1;
                            avg += Output[int2(xp, yp)].r;
                        }
                    }

                    if (cnt == 0) 
                        temp = Output[int2(x, y)].r;
					else
                        temp = avg / (float) cnt;

                    Output[int2(x, y)] = float4(0, 0, 0, 1);
                }

                Output[int2(x, y)] += float4(temp, 0, 0, 1);
            } // for y
        } // for x
    } // if
}

//-----------------------------------------------------------------------------
// Techinques
//-----------------------------------------------------------------------------

// 이거 넣어도 된다고 함
//ComputeShader C_CS = CompileShader(cs_5_0, CS());

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, Brush()));
    }
}