

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; // w = SpecPower
	float4 Reflect;
	float4 DiffuseMapST;
};

struct VertexIn_Common
{
	float3 PosL  : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex     : TEXCOORD;
	float4 Color : COLOR;
	float4 TangentL : TANGENT;
};

struct VertexOut_Common
{
	float4 PosH  : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 PosW : TEXCOORD1;
	float3 NormalW : TEXCOORD2;
	float3 TangentW : TEXCOORD3;
};

cbuffer cdPerFrame
{
	float3 g_CameraPosW;
	//Fog
	float4 g_linearFogColor;
	float g_linearFogStart;
	float g_linearFogRange;
	float g_timeDelta;
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
	float obj_ClipOff;
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D g_diffuseMap;
Texture2D g_normalMap;

SamplerState samCommon
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 6;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
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
	//
	float3 tangentL = vin.TangentL.xyz * vin.TangentL.w;//得到带朝向得法线
	vout.TangentW = mul(float4(tangentL, 1.0f), obj_MatWorld).xyz;//得到世界空间切线
    return vout;
}

//通用纹理采样
float4 tex2D(Texture2D map,float2 uv)
{
	return map.Sample(samCommon, uv);
}

//计算线性雾
float3 CalcuLinearFog(float3 posW,float3 inputColor)
{
	float disToEye = length(posW - g_CameraPosW);
	float s = saturate( (disToEye - g_linearFogStart) / g_linearFogRange );
	float3 ret = (1 - s) * inputColor + s * g_linearFogColor.rgb;
	return ret;
}

//将法线从切换空间转换到世界空间
float3 NormalTangent2WorldSpace(float3 normalSample,float3 unitNormalW,float3 tangentW)
{
	float3 normalT = normalize(2.0f * normalSample - 1.0f);//将法线贴图采样结果从[0,1]转换到[-1,1]
	float3 N = unitNormalW;
	float3 T = normalize(tangentW - dot(tangentW,N) * N);//做一个Reject操作，确保切线垂直于法线
	float3 B = normalize(cross(N,T));//计算世界空间下得副切线
	float3x3 TBN = float3x3(T,B,N);
	float3 normalW = normalize(mul(normalT,TBN));//得到世界空间下得切线向量
	return normalW;
}

float2 ndc_xy_to_uv(float2 ndcxy) 
{ 
	float2 uv = ndcxy * float2(0.5, -0.5) + float2(0.5, 0.5);
	return uv;
}