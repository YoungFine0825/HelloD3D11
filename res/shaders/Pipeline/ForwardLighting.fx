
struct Light
{
	float4 color;
	float3 positionW;
	float intensity;
	float3 attenuation;
	float range;
	float3 directionW;
	float spot;
};

cbuffer cdForwardLighting
{
	Light g_ParallelLight;
	Light g_PointLights[8];
	Light g_SpotLights[8];
}

void ComputeParallelLight(Light light,Material mat,float3 normalW,float3 viewDirW,
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
    CalcuDiffuseSpeacularColor(
        float3(0,0,0),
        normalize(light.directionW) * -1,
        normalW,
        viewDirW,
        light.color.rgb,
        mat.Diffuse.rgb,
        mat.Specular.rgb,
        mat.Specular.w,
        diffuseColor,
        specularColor
    );
}

void ComputePhongLight(float3 fragPosW,float3 normalW,Material mat,float isReceiveShadow,
	out float3 ambientColor,
	out float3 diffuseColor,
	out float3 specularColor
)
{
    float3 N = normalize(normalW);
	float3 viewDir = normalize(g_CameraPosW - fragPosW);
    //
	float3 A,D,S = float3(0,0,0);
	ambientColor = float3(0,0,0);
	diffuseColor = float3(0,0,0);
	specularColor = float3(0,0,0);
    //
    float litIntensity = g_ParallelLight.intensity;
    float shadowFactor = CalcParallelLightShadowFactor_CSM(fragPosW);
	shadowFactor = 1.0f - shadowFactor / litIntensity;
	shadowFactor *= isReceiveShadow;
    //计算平行光
	ComputeParallelLight(g_ParallelLight,mat,N,viewDir,A,D,S);
	ambientColor += A;
	diffuseColor += lerp(D,D * 0.5f,shadowFactor) * litIntensity;
	specularColor += lerp(S,S * 0.0f,shadowFactor) * litIntensity;
}