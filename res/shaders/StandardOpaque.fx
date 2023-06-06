#include "Common.fx"
#include "Pipeline/DeferredShadingDefine.fx"

VertexOut_Common VS(VertexIn_Common vin)
{
	VertexOut_Common vout = VertexShader_Common(vin);
    return vout;
}

float4 PS(VertexOut_Common pin,uniform bool gUseLinearFog,uniform bool gUseShadow) : SV_Target
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
        SetPixelShader( CompileShader( ps_5_0, PS(false,false) ) );
    }
}

#include "Pipeline/GBufferOpaque.fx"
