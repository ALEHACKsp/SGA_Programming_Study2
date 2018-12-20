//-----------------------------------------------------------------------------
// Compute Shader
//-----------------------------------------------------------------------------

static const int MaxBlurRadius = 8;
int BlurRadius = 5;

float Weights[MaxBlurRadius * 2 + 1];

static const int N = 256; // 그룹 스레드 크기
static const int CacheSize = (N + 2 * MaxBlurRadius); // 공유 자원의 사이즈가 될 꺼
// 한 그룹당 32kb가 최대임 넘어서면 그룹이 자동으로 열려서 2배로 느려짐 
// 24kb로 기준으로 한다고함 8kb정도는 다른데 쓰일 수 있어서 24kb 쓰라고 권고하고 있다고함
groupshared float4 Cache[CacheSize]; // 그룹 안에 스레드 메모리 공유 하기 위해서 씀, 공유 메모리

Texture2D Input;
RWTexture2D<float4> Output;

[numthreads(N, 1, 1)]
void Horizontal(int3 groupThreadId : SV_GroupThreadId, int3 dispatchThreadId : SV_DispatchThreadId)
{
    if (groupThreadId.x < BlurRadius)
    {
        int x = max(dispatchThreadId.x - BlurRadius, 0);
        Cache[groupThreadId.x] = Input[int2(x, dispatchThreadId.y)];

    }
    
    if (groupThreadId.x >= N - BlurRadius)
    {
        int x = min(dispatchThreadId.x + BlurRadius, Input.Length.x - 1);

        Cache[groupThreadId.x + 2 * BlurRadius] = Input[int2(x, dispatchThreadId.y)];
    }

    Cache[groupThreadId.x + BlurRadius] = Input[min(dispatchThreadId.xy, Input.Length.xy - 1)];
    GroupMemoryBarrierWithGroupSync();

    float4 color = 0;
    //[unroll]
    for (int i = -BlurRadius; i <= BlurRadius; i++)
    {
        int k = groupThreadId.x + BlurRadius + i;
        color += Weights[i + BlurRadius] * Cache[k];
    }

    Output[dispatchThreadId.xy] = color;
}

[numthreads(1, N, 1)]
void Vertical(int3 groupThreadId : SV_GroupThreadId, int3 dispatchThreadId : SV_DispatchThreadId)
{
    if (groupThreadId.y < BlurRadius)
    {
        int y = max(dispatchThreadId.y - BlurRadius, 0);
        Cache[groupThreadId.y] = Input[int2(dispatchThreadId.x, y)];

    }
    
    if (groupThreadId.y >= N - BlurRadius)
    {
        int y = min(dispatchThreadId.y + BlurRadius, Input.Length.y - 1);

        Cache[groupThreadId.y + 2 * BlurRadius] = Input[int2(dispatchThreadId.x, y)];
    }

    Cache[groupThreadId.y + BlurRadius] = Input[min(dispatchThreadId.xy, Input.Length.xy - 1)];
    GroupMemoryBarrierWithGroupSync();

    float4 color = 0;
    //[unroll]
    for (int i = -BlurRadius; i <= BlurRadius; i++)
    {
        int k = groupThreadId.y + BlurRadius + i;
        color += Weights[i + BlurRadius] * Cache[k];
    }

    Output[dispatchThreadId.xy] = color;
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
        SetComputeShader(CompileShader(cs_5_0, Horizontal()));
    }

    pass P1
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, Vertical()));
    }
}