struct VertexOut_GenShadowMap
{
	float4 PosH  : SV_POSITION;
	float4 PosClip : TEXCOORD0;
};

VertexOut_GenShadowMap VS_GenShadowMap(VertexIn_Common vin)
{
	VertexOut_GenShadowMap vout;
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), obj_MatMVP);
	vout.PosClip = vout.PosH;
	//
    return vout;
}

float4 PS_GenShadowMap(VertexOut_GenShadowMap pin) : SV_Target
{
	float depth = pin.PosClip.z / pin.PosClip.w;
    return float4(depth,depth,depth,1);
}

struct VertexOut_AlphaTestShowCaster
{
	float4 PosH  : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float4 PosClip : TEXCOORD1;
};

VertexOut_AlphaTestShowCaster VS_AlphaTestShadowCaster(VertexIn_Common vin)
{
	VertexOut_AlphaTestShowCaster vout;
	vout.PosH = mul(float4(vin.PosL, 1.0f), obj_MatMVP);
	vout.TexCoord = vin.Tex.xy;
	vout.PosClip = vout.PosH;
    return vout;
}

float4 PS_AlphaTestShadowCaster(VertexOut_AlphaTestShowCaster pin) : SV_Target
{
	float depth = pin.PosClip.z / pin.PosClip.w;
	float4 texColor = tex2D(g_diffuseMap, pin.TexCoord);
    clip(texColor.a - obj_ClipOff);
    return float4(depth, depth, depth, 1);
}

RasterizerState RS_GenShadowMap
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