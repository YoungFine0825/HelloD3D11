cbuffer cbPerObject
{
	float4x4 gMVP; 
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D g_diffuseMap;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn_Common
{
	float3 PosL  : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex     : TEXCOORD;
	float4 Color : COLOR;
	float3 TangentL : TANGENT;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

VertexOut VS(VertexIn_Common vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gMVP);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;

    //
	vout.TexCoord = vin.Tex;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    // Default to multiplicative identity.
    float4 albedo = pin.Color;
	// Sample texture.
	albedo.rgb = g_diffuseMap.Sample( samAnisotropic, pin.TexCoord ).rgb;
	//
    return albedo;
}

technique11 Default
{
    pass P0
    {
		SetDepthStencilState(0, 0);
		SetBlendState(0, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}