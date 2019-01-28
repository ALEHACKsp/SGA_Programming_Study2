struct Bone
{
    matrix BindPose;
};

StructuredBuffer<Bone> BoneBuffer;

ByteAddressBuffer Vertex;
RWByteAddressBuffer Result;

struct VertexTextureNormalTangentBlend
{
    float3 Position;
    float2 Uv;
    float3 Normal;
    float3 Tangent;
    float4 BlendIndices;
    float4 BlendWeights;
};

void Skinning(inout VertexTextureNormalTangentBlend vertex)
{
    float4x4 m;
    float3x3 m3;
	
    float weightSum = 0.0f;

    float3 position = 0;
	float3 normal = 0, tangent = 0;

	//[unroll]
 //   for (uint i = 0; ((i < 4) && (weightSum < 1.0f)); i++)
 //   {
 //       m = BoneBuffer[(uint) vertex.BlendIndices[i]].BindPose;
 //       m3 = (float3x3) m;

 //       position += mul(float4(vertex.Position, 1), m) * vertex.BlendWeights[i];
 //       normal += mul(vertex.Normal, m3) * vertex.BlendWeights[i];
 //       tangent += mul(vertex.Tangent, m3) * vertex.BlendWeights[i];

 //       weightSum += vertex.BlendWeights[i];
 //   }

    float4x4 transform = 0;
    transform += mul(vertex.BlendWeights.x, BoneBuffer[(uint) vertex.BlendIndices.x].BindPose);
    transform += mul(vertex.BlendWeights.y, BoneBuffer[(uint) vertex.BlendIndices.y].BindPose);
    transform += mul(vertex.BlendWeights.z, BoneBuffer[(uint) vertex.BlendIndices.z].BindPose);
    transform += mul(vertex.BlendWeights.w, BoneBuffer[(uint) vertex.BlendIndices.w].BindPose);

    vertex.Position = mul(float4(vertex.Position, 1), transform).xyz;
    //vertex.Normal = mul(float4(vertex.Normal, 1), transform).xyz;
    //vertex.Tangent = mul(float4(vertex.Tangent, 1), transform).xyz;

	//// any 0이 아니면 true 0이면 false 리턴
 //   bool w = any(weightSum);
 //   vertex.Position = w ? position : vertex.Position;
 //   vertex.Normal = w ? normal : vertex.Normal;
 //   vertex.Tangent = w ? tangent : vertex.Tangent;
}

int size = 19;

[numthreads(128, 1, 1)]
void CS(uint3 dispatchId : SV_DispatchThreadID)
{
    int fetchAddress = dispatchId.x * 4 * size;
    
    VertexTextureNormalTangentBlend vertex;

    // Vertex	Uv	Normal	Tangent	BlendIndices	BlendWeight
    //   3      2     3       3        4               4
    //   0     12	 20      32		  44			  60			76
    vertex.Position = asfloat(Vertex.Load3(fetchAddress + 0));
    vertex.Uv = asfloat(Vertex.Load2(fetchAddress + 12));
    vertex.Normal = asfloat(Vertex.Load3(fetchAddress + 20));
    vertex.Tangent = asfloat(Vertex.Load3(fetchAddress + 32));
    vertex.BlendIndices = asfloat(Vertex.Load4(fetchAddress + 44));
    vertex.BlendWeights = asfloat(Vertex.Load4(fetchAddress + 60));

    Skinning(vertex);

    Result.Store3(fetchAddress + 0,  asuint(vertex.Position));
    Result.Store2(fetchAddress + 12, asuint(vertex.Uv));
    Result.Store3(fetchAddress + 20, asuint(vertex.Normal));
    Result.Store3(fetchAddress + 32, asuint(vertex.Tangent));
    Result.Store4(fetchAddress + 44, asuint(vertex.BlendIndices));
    Result.Store4(fetchAddress + 60, asuint(vertex.BlendWeights));
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}