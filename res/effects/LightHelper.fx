
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

float LambertDiffuse(float3 lightDirW,float3 normalW)
{
	float NdotL = dot(lightDirW,normalW) * 0.5f + 0.5f;//半兰伯特漫反射系数
	//float NdotL = max(0,dot(lightDirW,normalW));//兰伯特漫反射系数
	return NdotL;
}



void CalcuDiffuseSpeacularColor(float3 lightColor,Material mat,float3 L,float3 normalW,float3 viewDir,
	out float3 diffuseColor,
	out float3 specularColor
	)
{
	diffuseColor = LambertDiffuse(L,normalW) * mat.Diffuse.rgb * lightColor;
	//
	float3 reflect = normalW * max(0,dot(normalW,L)) * 2 - L;
	float specularFactor = pow( max(0,dot(reflect,viewDir)) , mat.Specular.w);
	specularColor = specularFactor * mat.Specular.rgb * lightColor;
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

//计算平行光
void CalcuParallelLight(ParallelLight light,Material mat,float3 normalW,float3 viewDir,
	out float3 ambientColor,
	out float3 diffuseColor,
	out float3 specularColor
	)
{
	// Initialize outputs.
	ambientColor = float3(0.0f, 0.0f, 0.0f);
	diffuseColor = float3(0.0f, 0.0f, 0.0f);
	specularColor = float3(0.0f, 0.0f, 0.0f);
	//
	float3 L = normalize(light.directionW * -1);
	float3 lightColor = light.color.rgb * light.intensity;
	//
	diffuseColor = (lightColor * mat.Diffuse.rgb) * LambertDiffuse(L,normalW);
	//
	float3 reflect = L - normalW * max(0,dot(normalW,L)) * 2;
	float specularFactor = pow( max(0,dot(reflect,viewDir)) , mat.Specular.w);
	specularColor = (lightColor * mat.Specular.rgb) * specularFactor;
	
}


//计算点光源
void CalcuPointLight(PointLight light,Material mat,float3 normalW,float3 viewDir,float3 posW,
	out float3 ambientColor,
	out float3 diffuseColor,
	out float3 specularColor
	)
{
	// Initialize outputs.
	ambientColor = float3(0.0f, 0.0f, 0.0f);
	diffuseColor = float3(0.0f, 0.0f, 0.0f);
	specularColor = float3(0.0f, 0.0f, 0.0f);
	//
	float3 L = normalize(light.positionW - posW);
	float d = length(posW - light.positionW) / max(light.range,0.01);
	float3 att = light.attenuation;
	float scale = 1 / (att.x + att.y * d + att.z * pow(d,2)) * windownFunction(d,1);
	float3 lightColor = scale * light.color.rgb;
	//
	CalcuDiffuseSpeacularColor(lightColor,mat,L,normalW,viewDir,diffuseColor,specularColor);
	//
	diffuseColor *= light.intensity;
	specularColor *= light.intensity;
}



//计算聚光灯
void CalcuSpotLight(SpotLight light,Material mat,float3 normalW,float3 viewDir,float3 posW,
	out float3 ambientColor,
	out float3 diffuseColor,
	out float3 specularColor
	)
{
	// Initialize outputs.
	ambientColor = float3(0.0f, 0.0f, 0.0f);
	diffuseColor = float3(0.0f, 0.0f, 0.0f);
	specularColor = float3(0.0f, 0.0f, 0.0f);
	//
	float3 lit2Surface = normalize(posW - light.positionW);
	float3 litOrient = normalize(light.directionW);
	float distance2Surface = length(posW - light.positionW) / max(light.range,0.01);
	//
	float umbra = max(light.spot,0.01);
	float fi = acos(max(0,dot(litOrient,lit2Surface)));
	//距离衰减
	float inDistance = distance2Surface <= 1.0f;
	//
	float3 att = light.attenuation;
	float d = fi / umbra ;
	float scale = 1 / (att.x + att.y * d + att.z * pow(d,2)) * windownFunction(d,1);
	//
	float3 lightColor = inDistance * scale * light.color.rgb;
	//
	float3 L = normalize(light.positionW - posW);
	CalcuDiffuseSpeacularColor(lightColor,mat,L,normalW,viewDir,diffuseColor,specularColor);
	//
	diffuseColor *= light.intensity;
	specularColor *= light.intensity;
}