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

RasterizerState Depth
{
	// [From MSDN]
	// If the depth buffer currently bound to the output-merger stage has a UNORM format or
	// no depth buffer is bound the bias value is calculated like this: 
	//
	// Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope;
	//
	// where r is the minimum representable value > 0 in the depth-buffer format converted to float32.
	// [/End MSDN]
	// 
	// For a 24-bit depth buffer, r = 1 / 2^24.
	//
	// Example: DepthBias = 100000 ==> Actual DepthBias = 100000/2^24 = .006

	// You need to experiment with these values for your scene.
	DepthBias = 10000;
    DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};

technique11 Default
{
    pass P0
    {
		
		//SetDepthStencilState(0, 0);
        SetVertexShader( CompileShader( vs_5_0, VS_ShadowMap() ) );
		SetGeometryShader( NULL );
        SetPixelShader( NULL );
		SetRasterizerState(Depth);
    }
}