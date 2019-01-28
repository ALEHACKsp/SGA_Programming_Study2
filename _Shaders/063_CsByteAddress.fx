ByteAddressBuffer datas;
RWByteAddressBuffer result;

//int size = 4;
int size = 19;

[numthreads(128, 1, 1)]
void CS(uint3 dispatchId : SV_DispatchThreadID)
{
    int fetchAddress = dispatchId.x * 4 * size;
    float4 temp = asfloat(datas.Load4(fetchAddress));

	// �̷������� �θ����� ����
    //float tempX = datas.Load(fetchAddress);
    //float tempY = datas.Load(fetchAddress + 4);

    temp.x += temp.y + temp.z + temp.w / 3.3f;

	// ����� ������ uint������ ĳ�����������
    result.Store4(fetchAddress, asuint(temp));
}

[numthreads(128, 1, 1)]
void CS2(uint3 dispatchId : SV_DispatchThreadID)
{
    int fetchAddress = dispatchId.x * 4 * size;
    

    float3 pos = asfloat(datas.Load3(fetchAddress + 0));
    float2 uv = asfloat(datas.Load2(fetchAddress + 12));
    float3 nor = asfloat(datas.Load3(fetchAddress + 20));
    float3 tan = asfloat(datas.Load3(fetchAddress + 32));

    result.Store3(fetchAddress + 0, asuint(pos));
    result.Store3(fetchAddress + 20, asuint(nor));
    result.Store3(fetchAddress + 32, asuint(tan));
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CS()));
    }

    pass P1
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CS2()));
    }
}