#include "Common.fx"

cbuffer cbPerObjectAlphaTest
{
	float obj_ClipOff; 
};


float4 PS(VertexOut_Common pin,uniform bool gUseLinearFog) : SV_Target
{
	float4 texColor = tex2D(g_diffuseMap, pin.TexCoord);
	clip(texColor.a - obj_ClipOff);
	//
	float4 finalColor = texColor * obj_Material.Diffuse;
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
		SetRasterizerState(0);
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_Common() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(false) ) );
    }
}

technique11 UseLinearFog
{
    pass P0
    {
		SetRasterizerState(0);
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_Common() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true) ) );
    }
}