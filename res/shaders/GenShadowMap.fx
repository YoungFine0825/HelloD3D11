#include "Common.fx"
#include "ShadowMapDefine.fx"

float4 PS_ShadowMap(VertexOut_GenShadowMap pin) : SV_Target
{
    return float4(1,1,1,1);
}

technique11 Default
{
    pass P0
    {
		SetRasterizerState(RS_GenShadowMap);
		SetDepthStencilState(0, 0);
        SetVertexShader( CompileShader( vs_5_0, VS_GenShadowMap() ) );
		SetGeometryShader( NULL );
        SetPixelShader( NULL );
    }
}