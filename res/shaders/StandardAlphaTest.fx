#include "Common.fx"
#include "Pipeline/DeferredShadingDefine.fx"
#include "ShadowMapDefine.fx"

VertexOut_Common VS(VertexIn_Common vin)
{
	VertexOut_Common vout = VertexShader_Common(vin);
    return vout;
}

float4 PS(VertexOut_Common pin) : SV_Target
{
	float4 texColor = float4(1, 1, 1, 1);
	texColor = tex2D(g_diffuseMap, pin.TexCoord);
    clip(texColor.a - obj_ClipOff);
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

#include "Pipeline/GBufferAlphaTest.fx"
