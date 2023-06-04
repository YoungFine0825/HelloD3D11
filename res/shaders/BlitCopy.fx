#include "Common.fx"

struct VertexOut_BlitCopy
{
	float4 PosH  : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
};

//通用顶点着色器
VertexOut_BlitCopy VertexShader_BlitCopy(VertexIn_Common vin)
{
	VertexOut_BlitCopy vout;
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), obj_MatMVP);
    //
	vout.TexCoord = vin.Tex.xy;
	//
    return vout;
}

float4 PS(VertexOut_BlitCopy pin) : SV_Target
{
	float4 texColor = tex2D(g_diffuseMap, pin.TexCoord);
    return texColor;
}

DepthStencilState DSS
{
	DepthEnable = false;
	DepthFunc = Always;
};

RasterizerState RS
{
	DepthClipEnable = false;
};

technique11 Default
{
    pass P0
    {
		SetRasterizerState( RS );
		SetDepthStencilState(DSS, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_BlitCopy() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}

technique11 EnableDepthStencil
{
    pass P0
    {
		SetRasterizerState( 0 );
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_BlitCopy() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}