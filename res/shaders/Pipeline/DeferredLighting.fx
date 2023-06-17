#include "../Common.fx"
#include "DeferredShadingDefine.fx"
#include "PhongModel.fx"
#include "ShadowMapping.fx"

cbuffer cdDefeffedLighting
{
	float4 litColor;
	float3 litPositionW;
	float litIntensity;
	float3 litAttenuation;
	float litRange;
	float3 litDirectionW;
	float litSpot;
}

struct VertexOut_BlitCopy
{
	float4 PosH  : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
};

VertexOut_BlitCopy VertexShader_BlitCopy(VertexIn_Common vin)
{
	VertexOut_BlitCopy vout;
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), obj_MatMVP);
    //
	vout.TexCoord = vin.Tex.xy;
	//
    return vout;
}

float4 PS_Pass_ParallelLit(VertexOut_BlitCopy pin) : SV_Target
{
	float4 gbuffer0 = sampleGBuffer0(pin.TexCoord);
	float4 gbuffer1 = sampleGBuffer1(pin.TexCoord);
	float4 albedo = sampleGBuffer2(pin.TexCoord);
	//
	float3 posW = gbuffer0.xyz;
	float3 viewDir = normalize(g_CameraPosW - posW);
	float3 normalW = normalize(2.0f * gbuffer1.xyz - 1.0f);
	float specualrPower = gbuffer0.w;
	float isReceiveShadow = gbuffer1.w;
	//
	float3 diffuseColor = float3(1.0f,1.0f,1.0f);
	float3 specularColor = float3(1.0f,1.0f,1.0f);
	float3 D = float3(0.0f,0.0f,0.0f);
	float3 S = float3(0.0f,0.0f,0.0f);
	CalcuDiffuseSpeacularColor(
		float3(0,0,0),
		litDirectionW * -1,
		normalW,
		viewDir,
		litColor.rgb,
		diffuseColor,
		specularColor,
		specualrPower,
		D,
		S
	);
	float shadowFactor = CalcParallelLightShadowFactor_CSM(posW);
	shadowFactor = 1.0f - shadowFactor / litIntensity;
	shadowFactor *= isReceiveShadow;
	D = lerp(D,D * 0.5f,shadowFactor) * litIntensity;
	S = lerp(S,S * 0.0f,shadowFactor) * litIntensity;
	//
	float4 finalColor;
	finalColor.rgb = albedo.rgb * D.rgb + S.rgb;
	finalColor.a = 1.0f;
    return finalColor;
}

DepthStencilState DSS_ParallelLightPass
{
	DepthEnable = false;
	DepthFunc = Always;
};

RasterizerState RS_ParallelLightPass
{
	DepthClipEnable = false;
};


////////////////////////////////////////


struct VertexOut_PunctualLight
{
	float4 PosH  : SV_POSITION;
	float4 posH2 : TEXCOORD0;
};

VertexOut_PunctualLight VertexShader_PunctualLight(VertexIn_Common vin)
{
	VertexOut_PunctualLight vout;
	vout.PosH = mul(float4(vin.PosL, 1.0f), obj_MatMVP);
	vout.posH2 = vout.PosH;
    return vout;
}

float4 PS_Pass_PointLight(VertexOut_PunctualLight pin) : SV_Target
{
	float3 posH = pin.posH2.xyz / pin.posH2.w;
	float2 screenSpaceUV = ndc_xy_to_uv(posH.xy);
	float4 gbuffer0 = sampleGBuffer0(screenSpaceUV);
	float4 gbuffer1 = sampleGBuffer1(screenSpaceUV);
	float4 albedo = sampleGBuffer2(screenSpaceUV);
	//
	float3 posW = gbuffer0.xyz;
	float3 viewDir = normalize(g_CameraPosW - posW);
	float3 normalW = normalize(2.0f * gbuffer1.xyz - 1.0f);
	float specualrPower = gbuffer0.w;
	//
	float3 diffuseColor = float3(1.0f,1.0f,1.0f);
	float3 specularColor = float3(1.0f,1.0f,1.0f);
	float3 D = float3(1.0f,1.0f,1.0f);
	float3 S = float3(0.0f,0.0f,0.0f);
	//
	float3 lightColor = CalcuPointLightColor(posW,litPositionW,litColor.rgb,litRange,litAttenuation);
	//
	CalcuDiffuseSpeacularColor(
		posW,
		litPositionW,
		normalW,
		viewDir,
		lightColor,
		diffuseColor,
		specularColor,
		specualrPower,
		D,
		S
	);
	//
	D *= litIntensity;
	S *= litIntensity;
	//
	float4 finalColor;
	finalColor.rgb = albedo.rgb * D.rgb + S.rgb;
	finalColor.a = 1.0f;
    return finalColor;
}

float4 PS_Pass_SpotLight(VertexOut_PunctualLight pin) : SV_Target
{
	float3 posH = pin.posH2.xyz / pin.posH2.w;
	float2 screenSpaceUV = ndc_xy_to_uv(posH.xy);
	float4 gbuffer0 = sampleGBuffer0(screenSpaceUV);
	float4 gbuffer1 = sampleGBuffer1(screenSpaceUV);
	float4 albedo = sampleGBuffer2(screenSpaceUV);
	//
	float3 posW = gbuffer0.xyz;
	float3 viewDir = normalize(g_CameraPosW - posW);
	float3 normalW = normalize(2.0f * gbuffer1.xyz - 1.0f);
	float specualrPower = gbuffer0.w;
	//
	float3 diffuseColor = float3(1.0f,1.0f,1.0f);
	float3 specularColor = float3(1.0f,1.0f,1.0f);
	float3 D = float3(1.0f,1.0f,1.0f);
	float3 S = float3(0.0f,0.0f,0.0f);
	//
	float3 lightColor = CalcuSpotLightColor(posW,litPositionW,litColor.rgb,litRange,litAttenuation,litDirectionW,litSpot);
	//
	CalcuDiffuseSpeacularColor(
		posW,
		litPositionW,
		normalW,
		viewDir,
		lightColor,
		diffuseColor,
		specularColor,
		specualrPower,
		D,
		S
	);
	//
	D *= litIntensity;
	S *= litIntensity;
	//
	float4 finalColor;
	finalColor.rgb = albedo.rgb * D.rgb + S.rgb;
	finalColor.a = 1.0f;
    return finalColor;
}


/////////////////////////////////////////



BlendState BS_PunctualLight
{
	BlendEnable[0] = TRUE;
	SrcBlend[0] = One;
	DestBlend[0] = One;
	BlendOp[0] = ADD;
	SrcBlendAlpha[0] = ZERO;
	DestBlendAlpha[0] = ZERO;
	BlendOpAlpha[0] = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};

RasterizerState RS_PunctualLight_Inside
{
	CullMode = Front;
};

RasterizerState RS_PunctualLight_Outside
{
	CullMode = Back;
};

DepthStencilState DSS_PunctualLight_Inside
{
	DepthEnable = false;
	DepthWriteMask = Zero;//不写入深度缓存
};

DepthStencilState DSS_PunctualLight_Outside
{
	DepthEnable = true;
	DepthWriteMask = Zero;//不写入深度缓存
};


technique11 Default
{
    pass ParallelLightPass
    {
		SetRasterizerState( RS_ParallelLightPass );
		SetDepthStencilState(DSS_ParallelLightPass, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_BlitCopy() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Pass_ParallelLit() ) );
    }

	pass PointLightPass_Inside
    {
		SetRasterizerState( RS_PunctualLight_Inside );
		SetDepthStencilState(DSS_PunctualLight_Inside, 0);
		SetBlendState(BS_PunctualLight, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_PunctualLight() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Pass_PointLight() ) );
    }

	pass PointLightPass_Outside
    {
		SetRasterizerState( RS_PunctualLight_Outside );
		SetDepthStencilState(DSS_PunctualLight_Outside, 0);
		SetBlendState(BS_PunctualLight, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_PunctualLight() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Pass_PointLight() ) );
    }

	pass SpotLightPass_Inside
    {
		SetRasterizerState( RS_PunctualLight_Inside );
		SetDepthStencilState(DSS_PunctualLight_Inside, 0);
		SetBlendState(BS_PunctualLight, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_PunctualLight() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Pass_SpotLight() ) );
    }

	pass SpotLightPass_Outside
    {
		SetRasterizerState( RS_PunctualLight_Outside );
		SetDepthStencilState(DSS_PunctualLight_Outside, 0);
		SetBlendState(BS_PunctualLight, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_PunctualLight() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_Pass_SpotLight() ) );
    }
}