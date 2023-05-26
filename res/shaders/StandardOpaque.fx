#include "Common.fx"
#include "DeferredShadingDefine.fx"

VertexOut_Common VS(VertexIn_Common vin)
{
	VertexOut_Common vout = VertexShader_Common(vin);
    return vout;
}

float4 PS(VertexOut_Common pin,uniform bool gUseLinearFog,uniform bool gUseShadow) : SV_Target
{
	//BlinnPhong
	float3 ambientColor = float3(0,0,0);
	float3 diffuseColor = float3(0,0,0);
	float3 speacluarColor = float3(0,0,0);
	BlinnPhongLightingInWorldSpace(pin.NormalW,pin.PosW,obj_Material,gUseShadow,ambientColor,diffuseColor,speacluarColor);
	
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
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(false,false) ) );
    }
}


technique11 GBuffer
{
	pass GBufferPass0
	{
		SetRasterizerState(0);
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_GBuffer0() ) );	
	}

	pass GBufferPass1
	{
		SetRasterizerState(0);
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_GBuffer1() ) );	
	}

	pass GBufferPass2
	{
		SetRasterizerState(0);
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_GBuffer2() ) );	
	}

	pass GBufferPass3
	{
		SetRasterizerState(0);
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_GBuffer3() ) );	
	}
}

technique11 DeferredLighting
{
    pass P0
    {
		SetRasterizerState(0);
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(false,true) ) );
    }
}