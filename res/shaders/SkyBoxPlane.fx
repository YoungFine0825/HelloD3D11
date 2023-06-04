#include "Common.fx"

VertexOut_Common VS(VertexIn_Common vin)
{
	VertexOut_Common vout;
	// Transform to homogeneous clip space.
	float3 worldPos = mul(float4(vin.PosL, 1.0f), obj_MatWorld).xyz;
	worldPos = g_CameraPosW + worldPos;
	float4x4 trans = obj_MatProj * obj_MatView;
	vout.PosH = mul(float4(worldPos, 1.0f), obj_MatMVP);
    //
	vout.TexCoord = vin.Tex.xy * obj_Material.DiffuseMapST.xy + obj_Material.DiffuseMapST.zw;
	//
	vout.PosW = worldPos;
	//
	vout.NormalW = mul(float4(vin.NormalL, 0.0f), obj_MatNormalWorld).xyz;
	//
	vout.TangentW = float3(0,0,0);
    return vout;
}

SamplerState st
{
	Filter = MIN_MAG_MIP_LINEAR;//MIN_MAG_LINEAR_MIP_POINT;
	AddressU = MIRROR;
	AddressV = MIRROR;
};

float4 PS(VertexOut_Common pin) : SV_Target
{
	float4 finalColor;
	// Sample texture.
	finalColor.rgb = g_diffuseMap.Sample( st, pin.TexCoord ).rgb;
	//
	finalColor.w = 1.0f;
    return finalColor;
}

DepthStencilState DSS
{
	DepthEnable = true;
	DepthWriteMask = Zero;
	StencilEnable = true;
	StencilReadMask = 0;
	StencilWriteMask = 0;
	FrontFaceStencilFunc = EQUAL;
	FrontFaceStencilPass = Keep;
	FrontFaceStencilFail = Keep;
	BackFaceStencilFunc = EQUAL;
	BackFaceStencilPass = Keep;
	BackFaceStencilFail = Keep;
};

technique11 Default
{
    pass P0
    {
		SetRasterizerState(0);
		SetDepthStencilState(DSS, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}