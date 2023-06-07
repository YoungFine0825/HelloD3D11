
float4 PS_GBuffer0(VertexOut_Common pin) : SV_Target
{
    return float4(pin.PosW,obj_Material.Specular.w);
}

float4 PS_GBuffer1(VertexOut_Common pin) : SV_Target
{
	float3 normalW = normalize(pin.NormalW);
	float3 tangentW = normalize(pin.TangentW);
	float3 normalMapSample = g_normalMap.Sample(samLinear, pin.TexCoord).rgb;//采样得到切线空间得法线
	float3 bumppedNormalW = NormalTangent2WorldSpace(normalMapSample,normalW,tangentW);
    return float4(bumppedNormalW * 0.5f + 0.5f,obj_ReceiveShadow);
}

float4 PS_GBuffer2(VertexOut_Common pin) : SV_Target
{
    return tex2D(g_diffuseMap,pin.TexCoord);
}

float4 PS_GBuffer3(VertexOut_Common pin) : SV_Target
{
    return float4(pin.PosW,1.0f);
}

DepthStencilState DSS_GBufferOpaque
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
		SetDepthStencilState(DSS_GBufferOpaque, g_StencilRefOpaque);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_GBuffer0() ) );	
	}

	pass GBufferPass1
	{
		SetRasterizerState(0);
		SetDepthStencilState(DSS_GBufferOpaque, g_StencilRefOpaque);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_GBuffer1() ) );	
	}

	pass GBufferPass2
	{
		SetRasterizerState(0);
		SetDepthStencilState(DSS_GBufferOpaque, g_StencilRefOpaque);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_GBuffer2() ) );	
	}

	pass GBufferPass3
	{
		SetRasterizerState(0);
		SetDepthStencilState(DSS_GBufferOpaque, g_StencilRefOpaque);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_GBuffer3() ) );	
	}
}