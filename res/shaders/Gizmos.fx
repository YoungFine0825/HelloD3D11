cbuffer cbGizmos
{
	float4x4 obj_MatMVP; 
	float4 obj_Color;
};

struct VertexIn_Base
{
	float3 PosL  : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
};

VertexOut VS(VertexIn_Base vin)
{
	VertexOut vout;
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), obj_MatMVP);
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return obj_Color;
}

DepthStencilState DSS
{
	DepthEnable = false;
	DepthWriteMask = Zero;
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
		SetDepthStencilState(DSS, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}