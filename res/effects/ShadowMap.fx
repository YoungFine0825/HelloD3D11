#include "Common.fx"

struct VertexOut_GenShadowMap
{
	float4 PosH  : SV_POSITION;
};

VertexOut_GenShadowMap VS_ShadowMap(VertexIn_Common vin)
{
	VertexOut_GenShadowMap vout;
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), obj_MatMVP);
	//
    return vout;
}

float4 PS(VertexOut_GenShadowMap pin) : SV_Target
{
    return float4(1,1,1,1);
}


technique11 Default
{
    pass P0
    {
		SetDepthStencilState(0, 0);
        SetVertexShader( CompileShader( vs_5_0, VS_ShadowMap() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}