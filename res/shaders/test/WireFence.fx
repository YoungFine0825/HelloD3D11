#include "../Common.fx"
#include "../Pipeline/DeferredShadingDefine.fx"
#include "../ShadowMapDefine.fx"

VertexOut_Common VS(VertexIn_Common vin)
{
	VertexOut_Common vout = VertexShader_Common(vin);
    return vout;
}

float4 PS(VertexOut_Common pin) : SV_Target
{
	float4 texColor = float4(1, 1, 1, 1);
	texColor = tex2D(g_diffuseMap, pin.TexCoord);
    return texColor;
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
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }

    pass ShadowCaster
    {
		SetRasterizerState(RS_GenShadowMap);
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS_AlphaTestShadowCaster() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_AlphaTestShadowCaster() ) );
    }
}


float4 PS_GBuffer0(VertexOut_Common pin) : SV_Target
{
    float4 texColor = tex2D(g_diffuseMap,pin.TexCoord);
    clip(texColor.a - obj_ClipOff);
    return float4(pin.PosW,obj_Material.Specular.w);
}

float4 PS_GBuffer1(VertexOut_Common pin) : SV_Target
{
    float4 texColor = tex2D(g_diffuseMap,pin.TexCoord);
    clip(texColor.a - obj_ClipOff);
	float3 normalW = normalize(pin.NormalW);
    return float4(normalW * 0.5f + 0.5f,1.0f);
}

float4 PS_GBuffer2(VertexOut_Common pin) : SV_Target
{
    float4 texColor = tex2D(g_diffuseMap,pin.TexCoord);
    clip(texColor.a - obj_ClipOff);
    return texColor;
}

float4 PS_GBuffer3(VertexOut_Common pin) : SV_Target
{
    float4 texColor = tex2D(g_diffuseMap,pin.TexCoord);
    clip(texColor.a - obj_ClipOff);
    return float4(pin.PosW,1.0f);
}


DepthStencilState DSS_GBufferAlphaTest
{
	StencilEnable = true;
	StencilReadMask = 0xff;
	StencilWriteMask = 0xff;
	FrontFaceStencilFunc = Always;
	FrontFaceStencilPass = REPLACE;
	FrontFaceStencilFail = Keep;
	BackFaceStencilFunc = Always;
	BackFaceStencilPass = REPLACE;
	BackFaceStencilFail = Keep;
};

technique11 GBuffer
{
	pass GBufferPass0
	{
		SetRasterizerState(0);
		SetDepthStencilState(DSS_GBufferAlphaTest, g_StencilRefAlphaTest);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_GBuffer0() ) );	
	}

	pass GBufferPass1
	{
		SetRasterizerState(0);
		SetDepthStencilState(DSS_GBufferAlphaTest, g_StencilRefAlphaTest);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_GBuffer1() ) );	
	}

	pass GBufferPass2
	{
		SetRasterizerState(0);
		SetDepthStencilState(DSS_GBufferAlphaTest, g_StencilRefAlphaTest);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_GBuffer2() ) );	
	}

	pass GBufferPass3
	{
		SetRasterizerState(0);
		SetDepthStencilState(DSS_GBufferAlphaTest, g_StencilRefAlphaTest);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_GBuffer3() ) );	
	}
}
