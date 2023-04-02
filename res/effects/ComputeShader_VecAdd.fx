
struct Data
{
	float3 v1;
	float2 v2;
};

StructuredBuffer<Data> g_InputA;
StructuredBuffer<Data> g_InputB;
RWStructuredBuffer<Data> g_Output;

[numthreads(32, 1, 1)]
void CS(int3 dtid : SV_DispatchThreadID)
{
	g_Output[dtid.x].v1 = g_InputA[dtid.x].v1 + g_InputB[dtid.x].v1;
	g_Output[dtid.x].v2 = g_InputA[dtid.x].v2 + g_InputB[dtid.x].v2;
}


technique11 Default
{
    pass P0
    {
		SetVertexShader( NULL );
        SetPixelShader( NULL );
		SetComputeShader( CompileShader( cs_5_0, CS() ) );
    }
}