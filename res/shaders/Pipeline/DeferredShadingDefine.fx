// cbuffer cdDefeffedShading
// {

// }

Texture2D g_GBufferTex0;
Texture2D g_GBufferTex1;
Texture2D g_GBufferTex2;
Texture2D g_GBufferTex3;

SamplerState samplerGBuffer
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
};

DepthStencilState DSS_DrawBackgroundPass
{
	DepthEnable = true;
	DepthWriteMask = Zero;
	StencilEnable = true;
	StencilReadMask = 0xff;
	StencilWriteMask = 0xff;
	FrontFaceStencilFunc = EQUAL;
	FrontFaceStencilPass = Keep;
	FrontFaceStencilFail = Keep;
	BackFaceStencilFunc = EQUAL;
	BackFaceStencilPass = Keep;
	BackFaceStencilFail = Keep;
};

int g_StencilRefBackground = 0;
int g_StencilRefOpaque = 1;
int g_StencilRefAlphaTest = 2;

float4 sampleGBuffer0(float2 uv)
{
	return g_GBufferTex0.Sample(samplerGBuffer, uv);
}

float4 sampleGBuffer1(float2 uv)
{
	return g_GBufferTex1.Sample(samplerGBuffer, uv);
}

float4 sampleGBuffer2(float2 uv)
{
	return g_GBufferTex2.Sample(samplerGBuffer, uv);
}

float4 sampleGBuffer3(float2 uv)
{
	return g_GBufferTex3.Sample(samplerGBuffer, uv);
}