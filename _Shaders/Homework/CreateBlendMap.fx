//-----------------------------------------------------------------------------
// Compute Shader
//-----------------------------------------------------------------------------

#define RANDOM_IA 16807
#define RANDOM_IM 2147483647
#define RANDOM_AM (1.0f/float(RANDOM_IM))
#define RANDOM_IQ 127773u
#define RANDOM_IR 2836
#define RANDOM_MASK 123459876

struct NumberGenerator
{
    int seed; // Used to generate values.

	    // Generates the next number in the sequence.
    void Cycle()
    {
        seed ^= RANDOM_MASK;
        int k = seed / RANDOM_IQ;
        seed = RANDOM_IA * (seed - k * RANDOM_IQ) - RANDOM_IR * k;

        if (seed < 0) 
            seed += RANDOM_IM;

        seed ^= RANDOM_MASK;
    }

    // Cycles the generator based on the input count. Useful for generating a thread unique seed.
    // PERFORMANCE - O(N)
    void Cycle(const uint _count)
    {
        for (uint i = 0; i < _count; ++i)
            Cycle();
    }

    // Returns the current random float.
    float GetCurrentFloat()
    {
        Cycle();
        return RANDOM_AM * seed;
    }

    // Returns the current random int.
    int GetCurrentInt()
    {
        Cycle();
        return seed;
    }

    // Returns a random float within the input range.
    float GetRandomFloat(const float low, const float high)
    {
        float v = GetCurrentFloat();
        return low * (1.0f - v) + high * v;
    }

    // Sets the seed
    void SetSeed(const uint value)
    {
        seed = int(value);
        Cycle();
    }
};

NumberGenerator numberGenerator;

static const int N = 16; // 그룹 스레드 크기

float MaxHeight;

Texture2D<float4> Input;
RWTexture2D<float4> Output;

float grassFactor = 0.05f;
float stoneFactor = 0.4f;
float snowFactor = 0.75f;

[numthreads(N, N, 1)]
void Blend(int3 groupThreadId : SV_GroupThreadId, int3 dispatchThreadId : SV_DispatchThreadId)
{
    int width, height;
    Input.GetDimensions(width, height);

    float elevation = Input[dispatchThreadId.xy].r;

    float4 color = 0;
    float rand; 

	// dark green grass
    rand = grassFactor + numberGenerator.GetRandomFloat(-0.05f, 0.05f);
    if (elevation > MaxHeight * (grassFactor + rand))
        color.r = elevation / MaxHeight + numberGenerator.GetRandomFloat(-0.05f, 0.05f);

	// stone
    rand = stoneFactor + numberGenerator.GetRandomFloat(-0.15f, 0.15f);
    if (elevation > MaxHeight * (stoneFactor + rand))
        color.g = elevation / MaxHeight + numberGenerator.GetRandomFloat(-0.05f, 0.05f);

	// snow
    rand = snowFactor + numberGenerator.GetRandomFloat(-0.1f, 0.1f);
    if (elevation > MaxHeight * (snowFactor + rand))
        color.a = elevation / MaxHeight + numberGenerator.GetRandomFloat(-0.05f, 0.05f);

    Output[dispatchThreadId.xy] = color;
}

[numthreads(N, N, 1)]
void Smooth(int3 groupThreadId : SV_GroupThreadId, int3 dispatchThreadId : SV_DispatchThreadId)
{
    int width, height;
    Input.GetDimensions(width, height);

    float4 sum = Output[dispatchThreadId.xy];
    int num = 1;
	
    int minX = max(dispatchThreadId.x - 1, 0);
    int minY = max(dispatchThreadId.y - 1, 0);
    int maxX = min(dispatchThreadId.x + 2, width);
    int maxY = min(dispatchThreadId.y + 1, height);

    for (int x = minX; x < maxX; x++)
    {
		for (int y = minY; y < maxY; y++)
        {
            sum += Output[int2(x, y)];
            num += 1;
        }
    }

    Output[dispatchThreadId.xy] = sum / num;
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
        SetComputeShader(CompileShader(cs_5_0, Blend()));
    }
    pass P1
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, Smooth()));
    }
}