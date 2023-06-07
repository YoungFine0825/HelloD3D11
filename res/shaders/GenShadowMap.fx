#include "Common.fx"
#include "ShadowMapDefine.fx"


technique11 Default
{
    pass P0
    {
		SetRasterizerState(RS_GenShadowMap);
		SetDepthStencilState(0, 0);
        SetVertexShader( CompileShader( vs_5_0, VS_GenShadowMap() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS_GenShadowMap() ) );
    }
}