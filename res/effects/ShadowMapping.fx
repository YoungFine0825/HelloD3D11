
cbuffer cbShadowMapping
{
	float g_CSMSize;
	float g_CSMLevels;
	float4x4 g_pCSMVP0;
	float4x4 g_pCSMVP1;
}

//Parallel light shadow map
Texture2D g_parallelShadowMap;

SamplerComparisonState samShadow
{
	Filter   = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ComparisonFunc = LESS;
};


float2 ndcxy_to_uv(float2 ndcxy) 
{ 
	float2 uv = ndcxy * float2(0.5, -0.5) + float2(0.5, 0.5);
	return uv;
}

float is_in_ndc_space(float3 ndc)
{
	return step(ndc.x,1.0f) * step(-1.0f,ndc.x) * step(ndc.y,1.0f) * step(-1.0f,ndc.y) * step(ndc.z,1.0f) * step(0.0f,ndc.z);
}

float CSM_Depth_Sample(float2 uv,float level,float depth)
{
	float mapSize = 1 / g_CSMLevels;
	float2 uvMin = float2(level * mapSize,0);
	float2 uvMax = float2((level + 1) * mapSize,1.0);
	float2 clampedUV = uvMin + uv * float2(mapSize,1);
	// clampedUV.x = clamp(clampedUV.x,uvMin.x,uvMax.x);
	// clampedUV.y = clamp(clampedUV.y,uvMin.y,uvMax.y);
	float ret = g_parallelShadowMap.SampleCmpLevelZero(samShadow,clampedUV, depth).r;
	return step(1.0f,ret);
}

float CalcShadowFactor_CSM(float3 posW)
{
	float2 uv = float2(0,0);
	float level = g_CSMLevels - 1;
	float depth = 0;
	const float4x4 vps[2] = {g_pCSMVP0,g_pCSMVP1};
	[unroll]
	for(int m = 1; m >= 0; --m)
	{
		float4 posH = mul(float4(posW, 1.0f), vps[m]);
		posH.xyz /= posH.w;
		if(is_in_ndc_space(posH.xyz))
		{
			uv = ndcxy_to_uv(posH.xy);
			level = m;
			depth = posH.z;
		}
	}
	
	const float dx = 1.0f / g_CSMSize;
	const float dy = 1.0f / (g_CSMSize * g_CSMLevels);
	float percentLit = 0.0f;
	const float2 offsets[9] = 
	{
		float2(-dx,  -dy), float2(0.0f,  -dy), float2(dx,  -dy),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx,  +dy), float2(0.0f,  +dy), float2(dx,  +dy)
	};
	[unroll]
	for(int i = 0; i < 9; ++i)
	{
		percentLit += CSM_Depth_Sample(uv + offsets[i], level,depth);
	}
	float ret = percentLit / 9.0f;
	return ret;
}