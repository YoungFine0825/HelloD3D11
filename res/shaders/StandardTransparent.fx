#include "Common.fx"
#include "TransparentLighting.fx"


VertexOut_Common VS(VertexIn_Common vin)
{
	VertexOut_Common vout = VertexShader_Common(vin);
    return vout;
}

float4 PS(VertexOut_Common pin) : SV_Target
{
	float3 ambientColor = float3(0,0,0);
	float3 diffuseColor = float3(0,0,0);
	float3 speacluarColor = float3(0,0,0);
	//
	float3 normalW = normalize(pin.NormalW);
	float3 tangentW = normalize(pin.TangentW);
	float3 normalMapSample = g_normalMap.Sample(samLinear, pin.TexCoord).rgb;//采样得到切线空间得法线
	float3 bumppedNormalW = NormalTangent2WorldSpace(normalMapSample,normalW,tangentW);
	//
	ComputePhongLight(
		pin.PosW,
		bumppedNormalW,
		obj_Material,
		obj_ReceiveShadow,
		ambientColor,diffuseColor,speacluarColor
	);
	//
	float4 texColor = tex2D(g_diffuseMap, pin.TexCoord);
	//
	float4 finalColor;
	finalColor.xyz = texColor.rgb * diffuseColor + speacluarColor;
	finalColor.w = obj_Alpha;
	//
    return finalColor;
}

BlendState BS_StandardTransparent
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

DepthStencilState DSS_StandardTransparent
{
	DepthEnable = true;
	DepthWriteMask = Zero;//不写入深度缓存
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

technique11 Default
{
    pass P0
    {
		SetRasterizerState(0);
		SetDepthStencilState(DSS_StandardTransparent, 0);
		SetBlendState(BS_StandardTransparent, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
