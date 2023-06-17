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

float LambertDiffuse(float3 lightDirW,float3 normalW)
{
	float NdotL = dot(lightDirW,normalW) * 0.5f + 0.5f;//半兰伯特漫反射系数
	//float NdotL = max(0,dot(lightDirW,normalW));//兰伯特漫反射系数
	return NdotL;
}


void CalcuDiffuseSpeacularColor(float3 fragPosW,float3 litPositionW,float3 normalW,float3 viewDir,float3 lightColor,
    float3 diffuseColor,
    float3 specularColor,
    float specularPower,
	out float3 outDiffuseColor,
	out float3 outSpecularColor
	)
{
    float3 L = normalize(litPositionW - fragPosW);
	outDiffuseColor = LambertDiffuse(L,normalW) * diffuseColor.rgb * lightColor;
	//
	float3 reflectDir = normalize(normalW * dot(L,normalW) * 2 - L);
	float isSpecularVisible = step(0,dot(L,normalW));
	float specularFactor = pow( max(0,dot(reflectDir,viewDir)) , specularPower) * isSpecularVisible;
	outSpecularColor = specularFactor * specularColor * lightColor;
}

float3 CalcuPointLightColor(float3 fragPosW,float3 litPosW,float3 litColor,float litRange,float3 litAttenuation)
{
	float3 L = normalize(litPosW - fragPosW);
	float d = length(fragPosW - litPosW) / max(litRange,0.01);
	float3 att = litAttenuation;
	float scale = 1 / (att.x + att.y * d + att.z * pow(d,2)) * windownFunction(d,1);
	float3 ret = scale * litColor.rgb;
    return ret;
}

float3 CalcuSpotLightColor(float3 fragPosW,
    float3 litPositionW,float3 litColor,float litRange,float3 litAttenuation,
    float3 litDirectionW,float litSpot
    )
{
	float3 lit2Surface = normalize(fragPosW - litPositionW);
	float3 litOrient = normalize(litDirectionW);
	float distance2Surface = length(fragPosW - litPositionW) / max(litRange,0.01);
	float umbra = max(litSpot,0.01);
	float fi = acos(max(0,dot(litOrient,lit2Surface)));
	float inDistance = distance2Surface <= 1.0f;//距离衰减
	float3 att = litAttenuation;
	float d = fi / umbra ;
	float scale = 1 / (att.x + att.y * d + att.z * pow(d,2)) * windownFunction(d,1);
	float3 ret = inDistance * scale * litColor.rgb;
    return ret;
}