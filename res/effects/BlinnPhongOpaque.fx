struct ParallelLight
{
	float4 color;
	float3 directionW;
	float intensity;
};

struct PointLight
{
	float4 color;
	float3 positionW;
	float intensity;
	float3 attenuation;
	float range;
};

struct SpotLight
{
	float4 color;
	float3 positionW;
	float intensity;
	float3 attenuation;
	float range;
	float3 directionW;
	float spot;
};


cbuffer cdPerFrame
{
	float3 g_CameraPosW;
	//ParallelLight
	ParallelLight g_ParallelLight;
	PointLight g_PointLights[6];
	SpotLight g_SpotLights[6];
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
	float4 obj_AmbientColor;
	float4 obj_DiffuseColor;
	float4 obj_SpecularColor;
	float obj_SpecularPower;
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

struct VertexIn_Common
{
	float3 PosL  : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex     : TEXCOORD;
	float4 Color : COLOR;
	float3 TangentL : TANGENT;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 PosW : TEXCOORD1;
	float3 NormalW : TEXCOORD2;
};

VertexOut VS(VertexIn_Common vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), obj_MatMVP);

    //
	vout.TexCoord = vin.Tex;
	
	//
	vout.PosW = mul(float4(vin.PosL, 1.0f), obj_MatWorld).xyz;
	
	//
	vout.NormalW = mul(float4(vin.NormalL, 0.0f), obj_MatNormalWorld).xyz;
	
    return vout;
}

float LambertDiffuse(float3 lightDirW,float3 normalW)
{
	float NdotL = dot(lightDirW,normalW) * 0.5f + 0.5f;//半兰伯特漫反射系数
	//float NdotL = max(0,dot(lightDirW,normalW));//兰伯特漫反射系数
	return NdotL;
}

float3 DiffuseSpeacularColor(float3 lightColor,float3 L,float3 normalW)
{
	float3 diffuseColor = LambertDiffuse(L,normalW) * obj_DiffuseColor.rgb;
	float3 halfVector = normalize(normalW + L);
	float specularFactor = pow( max(0,dot(halfVector,normalW)) , obj_SpecularPower);
	float3 specularColor = obj_SpecularColor.rgb * specularFactor;
	return (diffuseColor + specularColor) * lightColor;
}

float windownFunction(float r,float rMax)
{
	float ret = 1 - pow(r / rMax,4);
	ret = clamp(ret,0,1);
	return ret;
}

float distanceFallofFunction(float r,float rMax)
{
	float ret = 1 - pow(r / rMax,2);
	ret = clamp(ret,0,1);
	return ret;
}

float3 calcuParallelLight(float3 normalW,float3 viewDir)
{
	float3 L = normalize(g_ParallelLight.directionW * -1);
	float3 lightColor = g_ParallelLight.color.rgb;
	//
	float3 ambientColor = lightColor * obj_AmbientColor.rgb;
	float3 diffuseColor = (lightColor * obj_DiffuseColor.rgb) * LambertDiffuse(L,normalW);
	float3 halfVector = normalize(normalW + L);
	float specularFactor = pow( max(0,dot(halfVector,normalW)) , obj_SpecularPower);
	float3 specularColor = (lightColor * obj_SpecularColor.rgb) * specularFactor;
	return (ambientColor + diffuseColor + specularColor) * g_ParallelLight.intensity;
}

float3 calcuPointLights(float3 normalW,float3 viewDir,float3 posW)
{
	float3 final;
	int i = 0;
	for(i = 0;i < 6;++i)
	{
		float3 L = normalize(g_PointLights[i].positionW - posW);
		float d = length(posW - g_PointLights[i].positionW) / g_PointLights[i].range;
		float3 att = g_PointLights[i].attenuation;
		float scale = 1 / (att.x + att.y * d + att.z * pow(d,2) ) * windownFunction(d,1);
		float3 lightColor = scale * g_PointLights[i].color.rgb;
		//
		final += DiffuseSpeacularColor(lightColor,L,normalW) * g_PointLights[i].intensity;
	}
	return final;
}

float3 calcuSpotLights(float3 normalW,float3 viewDir,float3 posW)
{
	float3 final;
	int i = 0;
	for(i = 0;i < 6;++i)
	{
		float3 lit2Surface = normalize(posW - g_SpotLights[i].positionW);
		float3 litOrient = normalize(g_SpotLights[i].directionW);
		float distance2Surface = length(posW - g_SpotLights[i].positionW) / g_SpotLights[i].range;
		//
		float umbra = g_SpotLights[i].spot;
		float fi = acos(max(0,dot(litOrient,lit2Surface)));
		//
		float inDistance = distance2Surface <= 1.0f;
		//
		float3 att = g_SpotLights[i].attenuation;
		float d = fi / umbra ;
		float scale = 1 / (att.x + att.y * d + att.z * pow(d,2) )* windownFunction(d,1);
		float3 lightColor = inDistance * scale * g_SpotLights[i].color.rgb;
		//
		float3 L = normalize(g_SpotLights[i].positionW - posW);
		final += DiffuseSpeacularColor(lightColor,L,normalW) * g_SpotLights[i].intensity;
	}
	return final;
}

float4 PS(VertexOut pin) : SV_Target
{
	float3 N = normalize(pin.NormalW);
	float3 viewDir = normalize(g_CameraPosW - pin.PosW);

	//计算光照
	float3 lightColor = calcuParallelLight(N,viewDir) + calcuPointLights(N,viewDir,pin.PosW) + calcuSpotLights(N,viewDir,pin.PosW);
	//lightColor = calcuPointLights(N,viewDir,pin.PosW) + calcuSpotLights(N,viewDir,pin.PosW);
	//
	float4 finalColor;
	finalColor.xyz = lightColor;
	finalColor.w = 1.0f;
    return finalColor;
}

technique11 Default
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}