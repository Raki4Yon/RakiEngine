#include "OBJShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);


PixelOutput main(GSOutput input)
{
    PixelOutput result;

	float4 texcolor = tex.Sample(smp, input.uv);
	
    result.pixel_color.rgb = float3(0.5f,0.5f,0.5f);
    result.pixel_color.a = 1.0f;
    //result.pixel_color = float4(1,0,0,1);
    result.normal.xyz = float3(input.normal.xyz / 2.0f) + 0.5f;
    result.normal.w = 1.0f;
    result.worldPos = input.worldPos;
    result.zColor = float4(0, 0, 0, 0);

    
    return result;
}

