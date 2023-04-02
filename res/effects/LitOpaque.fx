#include "Common.fx"

float4 PS(VertexOut_Common pin,uniform bool gUseLinearFog) : SV_Target
{
	//BlinnPhong
	float3 ambientColor = float3(0,0,0);
	float3 diffuseColor = float3(0,0,0);
	float3 speacluarColor = float3(0,0,0);
	BlinnPhongLightingInWorldSpace(pin.NormalW,pin.PosW,obj_Material,ambientColor,diffuseColor,speacluarColor);
	
	float4 texColor = float4(1, 1, 1, 1);
	texColor = tex2D(g_diffuseMap, pin.TexCoord);
	//
	float4 finalColor;
	finalColor.xyz = texColor.rgb * diffuseColor + speacluarColor;
	finalColor.w = 1.0f;
	//
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
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VertexShader_Common() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true) ) );
    }
}