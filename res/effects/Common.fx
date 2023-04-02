
#include "MaterialHelper.fx"
#include "LightHelper.fx"

struct VertexIn_Common
{
	float3 PosL  : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex     : TEXCOORD;
	float4 Color : COLOR;
	float3 TangentL : TANGENT;
};

struct VertexOut_Common
{
	float4 PosH  : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 PosW : TEXCOORD1;
	float3 NormalW : TEXCOORD2;
};

cbuffer cdPerFrame
{
	float3 g_CameraPosW;
	//ParallelLight
	ParallelLight g_ParallelLight;
	PointLight g_PointLights[6];
	SpotLight g_SpotLights[6];
	//Fog
	float4 g_linearFogColor;
	float g_linearFogStart;
	float g_linearFogRange;
}

cbuffer cbPerObject
{
	//Transform
	float4x4 obj_MatMVP; 
	float4x4 obj_MatWorld; 
	float4x4 obj_MatView; 
	float4x4 obj_MatProj; 
	float4x4 obj_MatNormalWorld; 
	//Material
	Material obj_Material;
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D g_diffuseMap;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};


//通用顶点着色器
VertexOut_Common VertexShader_Common(VertexIn_Common vin)
{
	VertexOut_Common vout;
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), obj_MatMVP);
    //
	vout.TexCoord = vin.Tex.xy * obj_Material.DiffuseMapST.xy + obj_Material.DiffuseMapST.zw;
	//
	vout.PosW = mul(float4(vin.PosL, 1.0f), obj_MatWorld).xyz;
	//
	vout.NormalW = mul(float4(vin.NormalL, 0.0f), obj_MatNormalWorld).xyz;
    return vout;
}



//通用纹理采样
float4 tex2D(Texture2D map,float2 uv)
{
	return map.Sample(samAnisotropic, uv);
}



//通用世界空间下，BlinnPhong光照计算
void BlinnPhongLightingInWorldSpace(float3 normalW,float3 posW,Material mat,
	out float3 ambientColor,
	out float3 diffuseColor,
	out float3 specularColor
	)
{
	float3 N = normalize(normalW);
	float3 viewDir = normalize(g_CameraPosW - posW);

	float3 A,D,S = float3(0,0,0);
	ambientColor = float3(0,0,0);
	diffuseColor = float3(0,0,0);
	specularColor = float3(0,0,0);
	//计算平行光
	CalcuParallelLight(g_ParallelLight,mat,N,viewDir,A,D,S);
	ambientColor += A;
	diffuseColor += D;
	specularColor += S;
	//计算点光源
	int lightIdx = 0;
	for(lightIdx = 0;lightIdx < 6;++lightIdx)
	{
		CalcuPointLight(g_PointLights[lightIdx],mat,N,viewDir,posW,A,D,S);
		ambientColor += A;
		diffuseColor += D;
		specularColor += S;
	}
	//计算聚光灯
	for(lightIdx = 0;lightIdx < 6;++lightIdx)
	{
		CalcuSpotLight(g_SpotLights[lightIdx],mat,N,viewDir,posW,A,D,S);
		ambientColor += A;
		diffuseColor += D;
		specularColor += S;
	}
}


//计算线性雾
float3 CalcuLinearFog(float3 posW,float3 inputColor)
{
	float disToEye = length(posW - g_CameraPosW);
	float s = saturate( (disToEye - g_linearFogStart) / g_linearFogRange );
	float3 ret = (1 - s) * inputColor + s * g_linearFogColor.rgb;
	return ret;
}