#include "Common.fx"

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

float4 PS(VertexOut_NormalMapping pin,uniform bool gUseLinearFog,uniform bool gUseShadow) : SV_Target
{
	float3 normalW = normalize(pin.NormalW);
	float3 tangentW = normalize(pin.TangentW);
	float3 normalMapSample = g_normalMap.Sample(samLinear, pin.TexCoord).rgb;
	float3 bumppedNormalW = NormalTangent2WorldSpace(normalMapSample,normalW,tangentW);
	//BlinnPhong
	float3 ambientColor = float3(0,0,0);
	float3 diffuseColor = float3(0,0,0);
	float3 speacluarColor = float3(0,0,0);
	BlinnPhongLightingInWorldSpace(bumppedNormalW,pin.PosW,obj_Material,gUseShadow,ambientColor,diffuseColor,speacluarColor);
	
	float4 texColor = float4(1, 1, 1, 1);
	texColor = tex2D(g_diffuseMap, pin.TexCoord);
	//
	float4 finalColor;
	finalColor.xyz = texColor.rgb * diffuseColor + speacluarColor;
	finalColor.w = 1.0f;
	//
	if(gUseLinearFog)
	{
		finalColor.rgb = CalcuLinearFog(pin.PosW,finalColor.rgb);
	}
	//
    return finalColor;
}

technique11 Default
{
    pass P0
    {
		SetRasterizerState(0);
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_NormalMapping() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(false,false) ) );

    }
}

technique11 UseLinearFog
{
    pass P0
    {
		SetRasterizerState(0);
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_NormalMapping() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true,false) ) );
    }
}

technique11 UseShadow
{
    pass P0
    {
		SetRasterizerState(0);		
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_NormalMapping() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(false,true) ) );
    }
}

technique11 UseLinearFogAndShadow
{
    pass P0
    {
		SetRasterizerState(0);
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_NormalMapping() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true,true) ) );
    }
}