#include "Common.fx"

cbuffer cbPerObjectTransparent
{
	float obj_Alpha; 
};

BlendState blend
{
	// Blending state for first render target.
	BlendEnable[0] = TRUE;
	SrcBlend[0] = SRC_COLOR;
	DestBlend[0] = INV_SRC_ALPHA;
	BlendOp[0] = ADD;
	SrcBlendAlpha[0] = ZERO;
	DestBlendAlpha[0] = ZERO;
	BlendOpAlpha[0] = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState DSS
{
	DepthEnable = true;
	DepthWriteMask = Zero;
	StencilEnable = true;
	StencilReadMask = 0xff;
	StencilWriteMask = 0xff;
	FrontFaceStencilFunc = Always;
	FrontFaceStencilPass = Incr;
	FrontFaceStencilFail = Keep;
	BackFaceStencilFunc = Always;
	BackFaceStencilPass = Incr;
	BackFaceStencilFail = Keep;
};

float4 PS(VertexOut_Common pin,uniform bool gUseLinearFog) : SV_Target
{
	float4 texColor = tex2D(g_diffuseMap, pin.TexCoord);
	//
	float4 finalColor = texColor * obj_Material.Diffuse;
	finalColor.a *= obj_Alpha;
	//计算雾效
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
		SetDepthStencilState(DSS, 0);
		SetBlendState(blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_Common() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(false) ) );
    }
}

technique11 UseLinearFog
{
    pass P0
    {
		SetDepthStencilState(DSS, 0);
		SetBlendState(blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_Common() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true) ) );
    }
}