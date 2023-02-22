// Orb Pixel Shader
// applies the texture passed through
// to the geometry 

// Was originally intended to be used alongide
// a tessellated sphere but repurposed as a standard 
// setting the texture pixel shader

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColour = texture0.Sample(sampler0, input.tex);
    
    return textureColour;
    
}