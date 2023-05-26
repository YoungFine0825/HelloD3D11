// cbuffer cdDefeffedShading
// {

// }

Texture2D gBufferTex0;
Texture2D gBufferTex1;
Texture2D gBufferTex2;
Texture2D gBufferTex3;

SamplerState samplerGBuffer
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
};


float4 PS_GBuffer0(VertexOut_Common pin) : SV_Target
{
    return float4(pin.PosW,1.0f);
}

float4 PS_GBuffer1(VertexOut_Common pin) : SV_Target
{
    return tex2D(g_diffuseMap,pin.TexCoord);
}

float4 PS_GBuffer2(VertexOut_Common pin) : SV_Target
{
    float3 normalW = normalize(pin.NormalW);
	float3 tangentW = normalize(pin.TangentW);
	float3 normalMapSample = g_normalMap.Sample(samLinear, pin.TexCoord).rgb;//采样得到切线空间得法线
	float3 bumppedNormalW = NormalTangent2WorldSpace(normalMapSample,normalW,tangentW);
    return float4(bumppedNormalW,1.0f);
}

float4 PS_GBuffer3(VertexOut_Common pin) : SV_Target
{
    return float4(pin.PosW,1.0f);
}