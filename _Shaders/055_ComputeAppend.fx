
struct Data
{
    int Id;
    int Value;
    int Sum;
};

ConsumeStructuredBuffer<int> Input;
AppendStructuredBuffer<Data> Output; // rw는 gpu에서 읽고 쓰기 가능하다는거 

[numthreads(64, 1, 1)] // data size보다 커야함
void CS(int3 id : SV_GroupThreadId)
{
    int data = Input.Consume();

    Data result;
    result.Id = id;
    result.Value = data;
    result.Sum = id + data;

    Output.Append(result);
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