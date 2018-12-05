
struct Data
{
    float3 V1;
    float3 V2;
};

StructuredBuffer<Data> Input1;
StructuredBuffer<Data> Input2;
RWStructuredBuffer<Data> Output; // rw는 gpu에서 읽고 쓰기 가능하다는거 

[numthreads(64, 2, 1)] // data size보다 커야함
void CS(int3 id : SV_GroupThreadId)
{
    Output[id.x].V1 = Input1[id.x].V1 + Input2[id.x].V1;
    Output[id.x].V2 = Input1[id.x].V2 + Input2[id.x].V2;
}


//-----------------------------------------------------------------------------
// Techinque
//-----------------------------------------------------------------------------

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}